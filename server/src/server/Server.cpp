#include <memory>
#include <iostream>
#include <cmath>
#include <string>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <pigpio.h>
#include <signal.h>
#include <grpcpp/grpcpp.h>

#include "Services.pb.h"
#include "Services.grpc.pb.h"

using grpc::ServerBuilder;
using grpc::Status;
using grpc::Server;
using grpc::ServerContext;

namespace 
{
    std::unique_ptr<Server> server;

    struct 
    { 
        std::mutex mtx;
        std::condition_variable driver_thread_cv;
        double left_track_intensity_percent{};
        double right_track_intensity_percent{};
        bool exit_requested{};
        bool dirty{};
    } server_state;
     
    void update_pins(double left_track_intensity_percent, double right_track_intensity_percent)
    {
        const double max = 255;
        const unsigned left_converted = (std::fabs(left_track_intensity_percent) / 100.0f) * max;
        const unsigned right_converted = (std::fabs(right_track_intensity_percent) / 100.0f) * max;

        std::cout << "Converted: " << left_converted << ", " << right_converted << '\n';

        if (left_track_intensity_percent > 0)
        {
            gpioPWM(21, left_converted);
            gpioPWM(20, 0);
        }
        else
        {
            gpioPWM(21, 0);
            gpioPWM(20, left_converted);
        }

        if (right_track_intensity_percent > 0)
        {
            gpioPWM(17, right_converted);
            gpioPWM(18, 0);
        }
        else
        {
            gpioPWM(17, 0);
            gpioPWM(18, right_converted);
        }
    }

    void driver_thread()
    { 
        gpioSetMode(20, PI_OUTPUT);
        gpioSetMode(21, PI_OUTPUT);
        gpioSetMode(17, PI_OUTPUT);
        gpioSetMode(18, PI_OUTPUT);
    
        while (true)
        {
            std::unique_lock lck(server_state.mtx);
            server_state.driver_thread_cv.wait(lck, []{
                return server_state.dirty || server_state.exit_requested;
            });
     
            if (server_state.exit_requested)
            {
                update_pins(0, 0);
                return;
            }
     
            update_pins(server_state.left_track_intensity_percent,
                        server_state.right_track_intensity_percent);
            server_state.dirty = false;
        }

        gpioTerminate();
    } 

    void signal_handler(int signal)
    {
        std::cout << "Caught signal: " << signal << std::endl;

        {
            std::lock_guard lck(server_state.mtx);
            server_state.exit_requested = true;
        }

        server_state.driver_thread_cv.notify_all();

        server->Shutdown();
    }
}

class SimpleRpcService final : public MasterService::Service
{
  Status execute_command(ServerContext* context, const CommandInput* request, CommandOutput* reply) override
  {
    
    std::lock_guard lck(server_state.mtx);
    
    {
        server_state.left_track_intensity_percent = request->left_track_intensity_percent();
        server_state.right_track_intensity_percent = request->right_track_intensity_percent();
        server_state.dirty = true;
    }

    server_state.driver_thread_cv.notify_all();

    std::cout << request->left_track_intensity_percent()
              << "% - " 
              << request->right_track_intensity_percent()
              << "%\n";

    return Status::OK;
  }
};

int main(int argc, char **argv)
{
    if (gpioInitialise() < 0)
    {
       std::cerr << "pigpio initialisation failed\n";
       return EXIT_FAILURE;
    }
  
    signal(SIGTERM, &signal_handler);
    signal(SIGINT, &signal_handler);
    signal(SIGQUIT, &signal_handler);

    auto dt = std::thread(driver_thread);

    std::string server_address("0.0.0.0:50051");
    SimpleRpcService service;
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    server = builder.BuildAndStart();
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
    dt.join();

    return EXIT_SUCCESS;
}
