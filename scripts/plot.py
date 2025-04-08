#!/usr/bin/env python3
# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

import argparse
import json
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np


def read_json(filename: Path) -> dict:
    with open(filename, "r") as f:
        data = json.load(f)
    return data


def read_dat(filename: Path) -> np.ndarray:
    data = np.loadtxt(filename, delimiter=" ")
    return data.transpose()


def get_param(param_dir: Path, name: str) -> dict:
    ret = {}
    ret.update(read_json(param_dir / "{}.obsgen.json".format(name)))
    ret.update(read_json(param_dir / "{}.init.json".format(name)))
    ret.update(read_json(param_dir / "{}.json".format(name)))
    return ret


def get_data(state_dir: Path, param: dict) -> dict:
    results = {}
    names = param["targets"].keys()
    for name in names:
        results[name] = {"all": {}, "mean": [], "std": []}
        for n in range(param["N"]):
            results[name]["all"][n] = []
    tmp = {}
    for t in range(0, param["t"] + 1):
        for name in names:
            tmp[name] = []
        for n in range(param["N"]):
            filename = "{}_{:04}_{:06}_{:06}.json".format(param["name"], n, t, t)
            if not (state_dir / filename).exists():
                continue
            data = read_json(state_dir / filename)
            for name in names:
                tmp[name].append(data["x"][param["targets"][name]["key"]])
        for name in names:
            if not tmp[name]:
                continue
            for n in range(param["N"]):
                results[name]["all"][n].append(np.array(tmp[name][n]))
            results[name]["mean"].append(np.array(tmp[name]).mean())
            results[name]["std"].append(np.array(tmp[name]).std())
    return results


def get_obs(obs_dir: Path, param: dict) -> dict:
    results = {}
    names = param["targets"].keys()
    for name in names:
        results[name] = []
    for t in range(0, param["t"] + 1):
        data = read_json(obs_dir / "{}_obs_{:06}.json".format(param["name"], t))
        for name in names:
            results[name].append(data["y"][param["targets"][name]["key"]])
    return results


def plot(output_dir: Path, results: dict, param: dict, show: bool = False, obs: dict = {}):
    for name in results.keys():
        info = param["targets"][name]
        result = results[name]
        t_list = np.linspace(0.0, param["t"] * param["dt"], len(result["mean"]))
        fig = plt.figure()
        ax = fig.subplots(1)
        ax.set_title(info["label"])
        for key in result["all"]:
            ax.plot(t_list, result["all"][key], color=info["color"], alpha=0.2)
        ax.plot(t_list, result["mean"], color=info["color"], label="Estimated")
        ax.scatter(t_list, obs[name], color="lime", label="Observation", zorder=10)
        ax.set_xlabel("time [s]")
        ax.legend()
        ax.set_xticks(t_list[::2])
        ax.set_xlim(t_list[0], t_list[-1])
        filename = output_dir / "{}.png".format(name)
        plt.savefig(filename)
        print("Result saved: {}".format(filename))
    if show:
        plt.show()


def main():
    parser = argparse.ArgumentParser(description="plot the estimation result")
    parser.add_argument("--show", action="store_true", help="show the plot")

    args = parser.parse_args()

    base = Path(__file__).parent.parent.resolve()
    state_dir = base / "output" / "state"
    obs_dir = base / "output" / "obs"
    param_dir = base / "param"

    param = get_param(param_dir, "lorenz63")
    param["targets"] = {
        "x": {"key": 0, "label": r"$x$", "color": "red"},
        "y": {"key": 1, "label": r"$y$", "color": "blue"},
        "z": {"key": 2, "label": r"$z$", "color": "orange"},
    }

    results = get_data(state_dir, param)
    for result in results.values():
        if not result["mean"] or not result["std"]:
            exit("No data found")
    obs = get_obs(obs_dir, param)
    plot(output_dir=state_dir, results=results, obs=obs, param=param, show=args.show)


if __name__ == "__main__":
    main()
