local_repository(
  name = "com_github_googlecartographer_cartographer",
  path = "/home/chris/catkin_ws/src/cartographer",
)

load("@com_github_googlecartographer_cartographer//:bazel/repositories.bzl", "cartographer_repositories")

cartographer_repositories()

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()