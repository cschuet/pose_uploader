#include "grpc++/impl/codegen/async_stream.h"
#include "grpc++/impl/codegen/proto_utils.h"
#include "cartographer/cloud/proto/map_builder_service.pb.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "google/protobuf/empty.pb.h"
#include "grpc++/grpc++.h"

DEFINE_string(cartographer_server_address, "127.0.0.1:50051", "Server address for the Cartographer gRPC process.");
DEFINE_bool(ssl_for_cartographer, false, "Enable SSL encryption for connection to Cartographer.");

namespace {

constexpr int kConnectionTimeoutInSecond = 10;
constexpr char kReceiveGlobalSlamOptimizationsMethodName[] = "/cartographer.cloud.proto.MapBuilderService/ReceiveGlobalSlamOptimizations";

void Run() {
    std::shared_ptr<::grpc::Channel> client_channel = ::grpc::CreateChannel(
            FLAGS_cartographer_server_address,
            FLAGS_ssl_for_cartographer
            ? ::grpc::SslCredentials(::grpc::SslCredentialsOptions())
            : ::grpc::InsecureChannelCredentials());
    std::chrono::system_clock::time_point deadline(
            std::chrono::system_clock::now() +
            std::chrono::seconds(kConnectionTimeoutInSecond));
    LOG(INFO) << "Connecting to Cartographer process @ " << FLAGS_cartographer_server_address;
    if (!client_channel->WaitForConnected(deadline)) {
        LOG(FATAL) << "Failed to connect to " << FLAGS_cartographer_server_address;
    }
    LOG(INFO) << "Connection established.";

    google::protobuf::Empty request;
    cartographer::cloud::proto::ReceiveGlobalSlamOptimizationsResponse response;
    const ::grpc::internal::RpcMethod kReceiveGlobalSlamOptimizations(kReceiveGlobalSlamOptimizationsMethodName,
                                                                      ::grpc::internal::RpcMethod::SERVER_STREAMING,
                                                                      client_channel);
    ::grpc::ClientContext context;
    ::grpc::CompletionQueue cq;
    void* finish_tag = (void*)1;
    void* connect_tag = (void*)2;
    void* read_tag = (void*)3;
    auto client_writer = ::grpc::internal::ClientAsyncReaderFactory<cartographer::cloud::proto::ReceiveGlobalSlamOptimizationsResponse>::Create(
            client_channel.get(), &cq, kReceiveGlobalSlamOptimizations, &context, request, true, connect_tag);
    ::grpc::Status status;
    client_writer->Finish(&status, finish_tag);

    while(true) {
        void* got_tag;
        bool ok = false;
        cq.Next(&got_tag, &ok);
        if (!ok) {
          LOG(ERROR) << "Error";
          break;
        }
        if (got_tag == connect_tag) {
          LOG(INFO) << "Connected.";
          client_writer->Read(&response, read_tag);
        } else if(got_tag == finish_tag) {
          LOG(INFO) << "Received EOF";
          break;
        } else if (read_tag) {
            LOG(INFO) << "Received message";
            LOG(INFO) << response.DebugString();
            client_writer->Read(&response, read_tag);
        }
    }
}

}  // namespace

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;
    google::SetUsageMessage(
            "\n\n"
            "This program uploads Cartographer poses to TFS.\n");
    google::ParseCommandLineFlags(&argc, &argv, true);
    if (FLAGS_cartographer_server_address.empty()) {
      google::ShowUsageWithFlagsRestrict(argv[0], "pose_uploader");
      return EXIT_FAILURE;
    }
    Run();
  //Setup clients for Receive*

  // Set Finish-tag and request read for both of them.

  // start send thread.

  // while not shutdown
  //   get next event from CQ
  //   parse and enqueue in out-queue (might require RPC to get poses for global)
  return 0;
}

/**
 * Send thread
 *
 * while not shutdown
 *   dequeue (blocking) from send queue and add to message
 *   check latency requirement
 *   if latency violated or max size exceeded send
 *
 * Latency
 *
 * check timestamp of current pose against timestamp of first pose in message.
 */
