cc_library(
    name = "simulator",
    srcs = ["src/simulator/simulator.cpp"],
    hdrs = ["src/simulator/simulator.h"],
    includes = ["src"],
    deps = [
        ":network",
        ":routing",
        ":message",
        ":traffic",
        ":metrics",
        ":utils",
    ],
)

cc_library(
    name = "network",
    srcs = [
        "src/network/network.cpp",
        "src/network/node.cpp",
        "src/network/link.cpp",
        "src/network/hypercube_network.cpp",
        "src/network/hypercube_node.cpp",
    ],
    hdrs = [
        "src/network/network.h",
        "src/network/node.h",
        "src/network/link.h",
        "src/network/virtual_channel.h",
        "src/network/hypercube_network.h",
        "src/network/hypercube_node.h",
        "src/network/hypercube_virtual_channel.h",
    ],
    includes = ["src"],
    deps = [":utils", ":message"],
)

cc_library(
    name = "routing",
    srcs = [
        "src/routing/duato_protocol.cpp",
        "src/routing/ecube_routing.cpp",
        "src/routing/duato_hypercube_protocol.cpp",
    ],
    hdrs = [
        "src/routing/routing_algorithm.h", 
        "src/routing/duato_protocol.h",
        "src/routing/ecube_routing.h",
        "src/routing/duato_hypercube_protocol.h",
    ],
    includes = ["src"],
    deps = [":utils", ":message", ":network"],
)

cc_library(
    name = "message",
    srcs = ["src/message/message.cpp", "src/message/packet.cpp"],
    hdrs = ["src/message/message.h", "src/message/packet.h"],
    includes = ["src"],
    deps = [":utils"],
)

cc_library(
    name = "traffic",
    srcs = [
        "src/traffic/uniform_traffic.cpp",
        "src/traffic/transpose_traffic.cpp", 
        "src/traffic/hotspot_traffic.cpp",
        "src/traffic/hypercube_uniform_traffic.cpp",
    ],
    hdrs = [
        "src/traffic/traffic_pattern.h",
        "src/traffic/uniform_traffic.h",
        "src/traffic/transpose_traffic.h",
        "src/traffic/hotspot_traffic.h",
        "src/traffic/hypercube_uniform_traffic.h",
    ],
    includes = ["src"],
    deps = [":utils"],
)

# Missing metrics library definition
cc_library(
    name = "metrics",
    srcs = [
        "src/metrics/metrics.cpp",
    ],
    hdrs = [
        "src/metrics/metrics.h",
    ],
    includes = ["src"],
    deps = [":utils"],
)

cc_library(
    name = "utils",
    srcs = [
        "src/utils/config.cpp",
        "src/utils/logger.cpp",
    ],
    hdrs = [
        "src/utils/config.h",
        "src/utils/logger.h",
        "src/utils/table_formatter.h",
    ],
    includes = ["src"],
)

# Simulator binary target with config file dependency

cc_binary(
    name = "simulator_binary",
    srcs = ["src/main.cpp"],
    includes = ["src"],
    data = [
        "config.json",
    ],
    deps = [
        ":simulator",
        ":network",
        ":routing",
        ":message",
        ":traffic",
        ":metrics",
        ":utils",
    ],
)