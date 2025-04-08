/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include "lorenz63.hh"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace douka_plugin
{
bool Lorenz63::set_option(const std::string &opt_file) {
  try {
    std::ifstream f{opt_file};
    this->param = nlohmann::json::parse(f);
  } catch (const std::ifstream::failure &e) {
    std::clog << e.what() << std::endl;
    return false;
  }

  return true;
}

bool Lorenz63::predict(std::vector<double> &s, const std::vector<double> &n) {
  this->history_filename = this->yieldHistoryFilename();
  std::vector<state> history;
  state x, dx;
  x.at(0) = s.at(0) + n.at(0);
  x.at(1) = s.at(1) + n.at(1);
  x.at(2) = s.at(2) + n.at(2);

  history.reserve(this->param.step + 1);
  history.emplace_back(x);
  for (int i = 0; i < this->param.step; i++) {
    this->system(x, dx, this->param.dt);

    x.at(0) += dx.at(0) * this->param.dt;
    x.at(1) += dx.at(1) * this->param.dt;
    x.at(2) += dx.at(2) * this->param.dt;
    history.emplace_back(x);
  }

  this->saveHistory(history);

  s.at(0) = x.at(0);
  s.at(1) = x.at(1);
  s.at(2) = x.at(2);
  return true;
}

void Lorenz63::system(const state &x, state &dx,
                                 [[maybe_unused]] const double dt) const {
  dx.at(0) = this->param.sigma * (x.at(1) - x.at(0));
  dx.at(1) = (this->param.rho - x.at(2)) * x.at(0) - x.at(1);
  dx.at(2) = x.at(0) * x.at(1) - this->param.beta * x.at(2);
}

void Lorenz63::saveHistory(const std::vector<state> &history) const {
  if (this->history_filename.empty()) {
    return;
  }

  if (!std::filesystem::exists(this->history_filename.parent_path())) {
    std::filesystem::create_directories(this->history_filename.parent_path());
  }

  std::ofstream fo{this->history_filename};
  if (!fo) {
    std::clog << this->history_filename << " could not open" << std::endl;
    return;
  }

  for (const auto &h : history) {
    fo << h.at(0) << " ";
    fo << h.at(1) << " ";
    fo << h.at(2) << std::endl;
  }
}

std::filesystem::path Lorenz63::yieldHistoryFilename() const {
  if (this->ctx == douka::PluginInterface::context::predict) {
    std::stringstream ss;
    ss << this->name;
    ss << "_" << std::setfill('0') << std::setw(4) << this->id;
    ss << "_" << std::setfill('0') << std::setw(6) << this->sys_tim;
    const auto base = std::filesystem::path("output");
    const auto file = std::filesystem::path(ss.str()) / "trajectory.dat";
    return base / "state" / file;
  } else if (this->ctx == douka::PluginInterface::context::obsgen) {
    std::stringstream ss;
    ss << this->name;
    ss << "_obs_";
    ss <<  std::setfill('0') << std::setw(6) << this->sys_tim;
    const auto base = std::filesystem::path("output");
    const auto file = std::filesystem::path(ss.str()) / "trajectory.dat";
    return base / "obs" / file;
  } else if (!this->history_filename.empty()) {
    return this->history_filename;
  }
  return std::filesystem::path();
}
} // namespace douka_plugin

#include <douka/plugin_register_macro.hh>
DOUKA_PLUGIN_REGISTER(douka_plugin::Lorenz63)
