workspace(name = "omni_simulator")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# 添加JSON库依赖（可选）
http_archive(
    name = "nlohmann_json",
    urls = ["https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz"],
    strip_prefix = "json-3.11.2",
    sha256 = "d69f9deb6a75e2580465c6c4c5111b89c4dc2fa94e3a85fcd2ffcd9a143d9273",
)