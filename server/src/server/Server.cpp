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
        std::condition_variable gpio_update_thread_cv;
        double left_track_intensity_percent{};
        double right_track_intensity_percent{};
        double turret_intensity_percent{};
        bool exit_requested{};
        bool turret_dirty{};
        bool tracks_dirty{};
    } server_state;
    
    void update_turret_pins(double turret_intensity_percent)
    {
        const double max = 255;
        const unsigned converted = (std::fabs(turret_intensity_percent) / 100.0f) * max;

        std::cout << "(Turret) Converted: " << converted << ", " << '\n';
        
        // TODO
        // gpioPWM(18, converted);
    }

    void update_track_pins(double left_track_intensity_percent, double right_track_intensity_percent)
    {
        const double max = 255;
        const unsigned left_converted = (std::fabs(left_track_intensity_percent) / 100.0f) * max;
        const unsigned right_converted = (std::fabs(right_track_intensity_percent) / 100.0f) * max;

        std::cout << "(Tracks) Converted: " << left_converted << ", " << right_converted << '\n';

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

    void gpio_update_loop()
    { 
        gpioSetMode(20, PI_OUTPUT);
        gpioSetMode(21, PI_OUTPUT);
        gpioSetMode(17, PI_OUTPUT);
        gpioSetMode(18, PI_OUTPUT);
    
        while (true)
        {
            std::unique_lock lck(server_state.mtx);
            server_state.gpio_update_thread_cv.wait(lck, []{
                return server_state.turret_dirty || server_state.tracks_dirty || server_state.exit_requested;
            });
     
            if (server_state.exit_requested)
            {
                update_track_pins(0, 0);
                return;
            }
    
            if (server_state.turret_dirty) 
            {
                server_state.turret_dirty = false;
                update_track_pins(server_state.left_track_intensity_percent, server_state.right_track_intensity_percent);
            }
            
            if (server_state.tracks_dirty)
            {
                server_state.tracks_dirty = false;
                update_turret_pins(server_state.turret_intensity_percent);
            }
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

        server_state.gpio_update_thread_cv.notify_all();

        server->Shutdown();
    }
}

class SimpleRpcService final : public MasterService::Service
{
  Status set_turret(ServerContext* context, const TurretInput* request, TurretOutput* reply) override
  {
    std::lock_guard lck(server_state.mtx);
    
    {
        server_state.turret_intensity_percent = request->intensity_percent();
        server_state.turret_dirty = true;
    }

    server_state.gpio_update_thread_cv.notify_all();

    std::cout << request->intensity_percent() << "%\n"; 

    return Status::OK;
  }

  Status set_tracks(ServerContext* context, const TracksInput* request, TracksOutput* reply) override
  {
    
    std::lock_guard lck(server_state.mtx);
    
    {
        server_state.left_track_intensity_percent = request->left_track_intensity_percent();
        server_state.right_track_intensity_percent = request->right_track_intensity_percent();
        server_state.tracks_dirty = true;
    }

    server_state.gpio_update_thread_cv.notify_all();

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

    auto gpio_update_thread = std::thread(gpio_update_loop);

    std::string server_address("0.0.0.0:50051");
    SimpleRpcService service;
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    server = builder.BuildAndStart();
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
    gpio_update_thread.join();

    return EXIT_SUCCESS;
}
