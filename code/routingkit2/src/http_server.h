#ifndef ROUTING_KIT2_HTTP_SERVER_H
#define ROUTING_KIT2_HTTP_SERVER_H

#include <string>
#include <vector>
#include <memory>
#include <utility>

namespace RoutingKit2{
    namespace http{
        struct Request{
            std::string resource;
            std::string body;
        };

        struct Response{
            std::string body;
            std::string mime_type;
            int status;

            Response()
                :status(200){}
            Response(std::string body)
                :body(std::move(body)){}
            Response(std::string body, int status)
                :body(std::move(body)), status(status){}
            Response(std::string body, std::string mime_type)
                :body(std::move(body)), mime_type(std::move(mime_type)), status(200){}
            Response(std::string body, std::string mime_type, int status)
                :body(std::move(body)), mime_type(std::move(mime_type)), status(status){}

        };

        struct Config{
            int port;
            int worker_count;
            bool install_int_signal_handler;
            int max_request_body_size;
            int max_request_resource_size;
            int timeout_in_seconds;
            bool print_exception_message_in_answer;

            Config();
        };

        namespace detail{

            typedef void (*RequestHandler)(int worker_id, int worker_count, void*user_data, const Request&in, Response&out);
            typedef void (*StartupHandler)(void*user_data);

            void run(
                Config config,
                RequestHandler request_handler,
                void*request_user_data,
                StartupHandler startup_handler,
                void*startup_user_data
            );

            template<class RequestCallback>
            void request_handler_entry(int worker_id, int worker_count, void*user_data, const Request&in, Response&out){
                const RequestCallback*handler = static_cast<const RequestCallback*>(user_data);
                (*handler)(worker_id, worker_count, in, out);
            }

            template<class StartupCallback>
            void startup_handler_entry(void*user_data){
                const StartupCallback*callback = static_cast<const StartupCallback*>(user_data);
                (*callback)();
            }
        }

        template<class RequestCallback, class StartupCallback>
        void run(Config config, const RequestCallback&request_handler, const StartupCallback&startup_callback){
            detail::run(
                config,
                &detail::request_handler_entry<RequestCallback>,
                const_cast<RequestCallback*>(&request_handler),
                &detail::startup_handler_entry<StartupCallback>,
                const_cast<StartupCallback*>(&startup_callback)
            );
        }

        template<class RequestCallback>
        void run(Config config, const RequestCallback&request_handler){
            run<RequestCallback>(config, request_handler, [](){});
        }

        template<class WorkerData, class ServerData, class RequestCallback, class StartupCallback>
        void run_with_worker_data(Config config, const ServerData&server_data, const RequestCallback&request_callback, const StartupCallback&startup_callback){
            std::vector<std::unique_ptr<WorkerData>>worker_data_list(config.worker_count);
            for(int i=0; i<config.worker_count; ++i)
                worker_data_list[i] = std::unique_ptr<WorkerData>(new WorkerData(i, config.worker_count, server_data));
            run(
                config,
                [&](int worker_id, int worker_count, const Request&request, Response&response){
                    request_callback(
                        worker_id, worker_count,
                        server_data, *worker_data_list[worker_id],
                        request, response
                    );
                },
                startup_callback
            );
        }


        template<class WorkerData, class ServerData, class RequestCallback>
        void run_with_worker_data(Config config, const ServerData&server_data, const RequestCallback&request_handler){
            run_with_worker_data<WorkerData, ServerData, RequestCallback>(config, server_data, request_handler, [](){});
        }
    }
}

#endif
