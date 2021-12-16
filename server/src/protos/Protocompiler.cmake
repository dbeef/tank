macro(compile_protos PROTO_FILES)

    foreach (proto_file IN LISTS PROTO_FILES)

        # Define name for output file
        set(compiled_name ${proto_file})
        string(REPLACE ".proto" "" compiled_name ${compiled_name})
        string(REPLACE ${PROTO_FILES_PATH} ${PROTOCOMPILER_OUTPUT_DIR} compiled_name ${compiled_name})

        set(compiled_proto_name_source "${compiled_name}.pb.cc")
        set(compiled_proto_name_header "${compiled_name}.pb.h")
        set(compiled_grpc_name_source "${compiled_name}.grpc.pb.cc")
        set(compiled_grpc_name_header "${compiled_name}.grpc.pb.h")

        list(APPEND COMPILED_FILES
                ${compiled_proto_name_source}
                ${compiled_proto_name_header}
                ${compiled_grpc_name_source}
                ${compiled_grpc_name_header}
        )

        execute_process(COMMAND which grpc_cpp_plugin OUTPUT_VARIABLE GRPC_CPP_PLUGIN)
        string(REGEX REPLACE "[ \t\r\n ]" "" GRPC_CPP_PLUGIN ${GRPC_CPP_PLUGIN})

        # Try to compile file with name passed in FILES list
        add_custom_command(
                OUTPUT
                ${compiled_proto_name_source}
                ${compiled_proto_name_header}
                ${compiled_grpc_name_source}
                ${compiled_grpc_name_header}
                COMMAND
                protoc
                ARGS
                --grpc_out ${PROTOCOMPILER_OUTPUT_DIR}
                --cpp_out ${PROTOCOMPILER_OUTPUT_DIR}
                -I ${PROTO_FILES_PATH}
                --plugin=protoc-gen-grpc="${GRPC_CPP_PLUGIN}"
                ${proto_file}
                DEPENDS
                ${proto_file}
        )

        execute_process(COMMAND which protoc-gen-grpc-web OUTPUT_VARIABLE GRPC_WEB_PLUGIN)
        string(REGEX REPLACE "[ \t\r\n ]" "" GRPC_WEB_PLUGIN ${GRPC_WEB_PLUGIN})

        execute_process(COMMAND 
            protoc
            -I ${PROTO_FILES_PATH}
            --plugin=protoc-gen-grpc=${GRPC_WEB_PLUGIN}
            --js_out=import_style=commonjs:${PROTOCOMPILER_OUTPUT_DIR}
            --grpc-web_out=import_style=commonjs,mode=grpcwebtext:${PROTOCOMPILER_OUTPUT_DIR}
            ${proto_file}
        )

        # FIXME: For some reason CMake decides to completely ignore this command -
        #        not even smallest trace in resulted makefiles.
        #        Using execute_process as a workaround, but it does not re-call protocompiler
        #        upon .proto files being modified.
        # add_custom_command(
                # OUTPUT
                # ${compiled_name}_pb.js
                # ${compiled_name}_grpc_web_pb.js
                # COMMAND
                # protoc
                # ARGS
                # -I ${PROTO_FILES_PATH}
                # --plugin=protoc-gen-grpc=${GRPC_WEB_PLUGIN}
                # --js_out=import_style=commonjs:${PROTOCOMPILER_OUTPUT_DIR}
                # --grpc-web_out=import_style=commonjs,mode=grpcwebtext:${PROTOCOMPILER_OUTPUT_DIR}
                # ${proto_file}
                # DEPENDS
                # ${proto_file}
        # ) 

    endforeach ()
endmacro()
