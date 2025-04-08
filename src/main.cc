/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include "lorenz63.hh"
#include <douka/io.hh>
#include <iostream>

struct Args {
  std::string plugin_param;
  std::string output;
  double x = 1.0;
  double y = 3.0;
  double z = 5.0;
};

void get_args(const int argc, char const *const argv[], Args &args) {
  const auto &show_help = [argv](std::ostream &os) {
    os << "Usage: " << std::filesystem::path(argv[0]).filename().string() << " [options]\n"
       << "Options:\n"
       << "  --plugin_param <file>  Specify the plugin parameter file\n"
       << "  --output <dir>         Specify the output directory\n"
       << "  --x <val>              Specify the value of initial x\n"
       << "  --y <val>              Specify the value of initial y\n"
       << "  --z <val>              Specify the value of initial z\n"
       << "  --help                 Show this help message\n";
  };

  enum class Context {
    none = 0,
    plugin_param,
    x,
    y,
    z,
    output,
  } ctx = Context::none;

  for (int i = 1; i < argc; ++i) {
    if (!strncmp(argv[i], "--", 2)) {
      ctx = Context::none;
      if (!strcmp(argv[i], "--plugin_param")) {
        ctx = Context::plugin_param;
      } else if (!strcmp(argv[i], "--output")) {
        ctx = Context::output;
      } else if (!strcmp(argv[i], "--x")) {
        ctx = Context::x;
      } else if (!strcmp(argv[i], "--y")) {
        ctx = Context::y;
      } else if (!strcmp(argv[i], "--z")) {
        ctx = Context::z;
      } else if (!strcmp(argv[i], "--help")) {
        show_help(std::cout);
        exit(EXIT_SUCCESS);
      } else {
        std::clog << "Invalid option: " << argv[i] << std::endl;
        exit(EXIT_FAILURE);
      }
    } else {
      switch (ctx) {
      case Context::plugin_param:
        args.plugin_param = argv[i];
        ctx = Context::none;
        break;
      case Context::output:
        args.output = argv[i];
        ctx = Context::none;
        break;
      case Context::x:
        try {
          args.x = std::stod(argv[i]);
        } catch (const std::invalid_argument &e) {
          std::clog << "Invalid argument for x: " << argv[i] << std::endl;
          exit(EXIT_FAILURE);
        }
        ctx = Context::none;
        break;
      case Context::y:
        try {
          args.y = std::stod(argv[i]);
        } catch (const std::invalid_argument &e) {
          std::clog << "Invalid argument for y: " << argv[i] << std::endl;
          exit(EXIT_FAILURE);
        }
        ctx = Context::none;
        break;
      case Context::z:
        try {
          args.z = std::stod(argv[i]);
        } catch (const std::invalid_argument &e) {
          std::clog << "Invalid argument for z: " << argv[i] << std::endl;
          exit(EXIT_FAILURE);
        }
        ctx = Context::none;
        break;
      default:
        std::clog << "Invalid option: " << argv[i] << std::endl;
        exit(EXIT_FAILURE);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  Args args;
  get_args(argc, argv, args);

  std::vector<double> state, noise;
  state.resize(3);
  noise.resize(3);

  douka_plugin::Lorenz63 model;

  /* Default parameters */
  model.param.sigma = 10.0;
  model.param.rho = 32.0;
  model.param.beta = 8 / 3;

  if (!args.plugin_param.empty()) {
    nlohmann::json plugin_param_json;
    if (!douka::io::read_json(args.plugin_param, plugin_param_json)) {
      return EXIT_FAILURE;
    }

    try {
      model.param = plugin_param_json;
    } catch (const nlohmann::json::exception &e) {
      std::clog << e.what() << std::endl;
      return EXIT_FAILURE;
    }
  }
  /* Those value will be overwritten */
  model.param.dt = 0.01;
  model.param.step = 10000;
  model.ctx = douka::PluginInterface::context::none;

  const auto output =
      args.output.empty() ? std::filesystem::current_path() : std::filesystem::path(args.output);
  if (!std::filesystem::exists(output)) {
    std::filesystem::create_directories(output);
  }

  model.history_filename = output / (model.name + "_trajectory.dat");

  state.at(0) = args.x;
  state.at(1) = args.y;
  state.at(2) = args.z;

  noise.at(0) = 0.0;
  noise.at(1) = 0.0;
  noise.at(2) = 0.0;

  model.predict(state, noise);

  std::cout << model.param << std::endl;
  std::cout << "x0 " << args.x << std::endl;
  std::cout << "y0 " << args.y << std::endl;
  std::cout << "z0 " << args.z << std::endl;

  return EXIT_SUCCESS;
}
