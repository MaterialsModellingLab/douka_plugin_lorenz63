/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __LORENZ63__
#define __LORENZ63__

#include <douka/plugin_interface.hh>
#include <nlohmann/json.hpp>
#include <ostream>
#include <string>
#include <vector>

namespace douka_plugin {
struct Param {
  double sigma;
  double rho;
  double beta;
  double dt;
  int step;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Param, sigma, rho, beta, dt, step);

  friend std::ostream &operator<<(std::ostream &os, const Param &param) {
    os << "sigma " << param.sigma << std::endl;
    os << "rho " << param.rho << std::endl;
    os << "beta " << param.beta << std::endl;
    os << "dt " << param.dt << std::endl;
    os << "step " << param.step;
    return os;
  }
};

class Lorenz63 : public douka::PluginInterface {
private:
  using state = std::array<double, 3>;

public:
  const std::string name = "lorenz63";
  struct Param param = {};
  std::filesystem::path history_filename;

public:
  Lorenz63() {}
  ~Lorenz63() {}
  bool set_option(const std::string &opt_file) override;
  bool predict(std::vector<double> &s, const std::vector<double> &n) override;

private:
  void system(const state &x, state &dx, double dt) const;
  void saveHistory(const std::vector<state> &history) const;
  std::filesystem::path yieldHistoryFilename() const;
};

} // namespace douka_plugin
#endif
