load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

cc_binary(
    name = "pose_uploader",
    srcs = ["main.cc"],
    deps = [
        "@com_github_gflags_gflags//:gflags",
        "@com_github_googlecartographer_cartographer//cartographer/cloud:cc_protos",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_glog//:glog",
    ],
)
