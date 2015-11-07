from random import choice
import json
import sys
import pymue
from collections import defaultdict

MAX_WAY = 9.3


def way_cost(way_length):
    if way_length <= 1:
        return way_length * 100
    elif way_length < MAX_WAY:
        return (way_length * 100) ** 2
    return sys.float_info.max


print("read data....")

pref = ""
if len(sys.argv):
    pref = "_%s" % sys.argv[1]

with open("teams%s.json" % pref, "r") as f:
    team_data = json.load(f)

with open("distances%s.json" % pref, "r") as f:
    distance_data = json.load(f)

print("map teams....")
team_map = dict()
team_map_reverse = dict()
for team in team_data:
    team_map[team["id"]] = team["idx"]
    team_map_reverse[team["idx"] = team["id"]

cnt = len(team_data)
teams = sorted(team_map.values())

print("build distance matrix...")
distance_matrix = pymue.DistanceMatrix(cnt)
for distance_run in distance_data:
    for src in distance_run:
        for dst in distance_run[src]:
            distance_matrix.set_cost(team_map[int(src)],
                                     team_map[int(dst)],
                                     way_cost(distance_run[src][dst]))

print("Minimum cost is  : %02f" % distance_matrix.min_cost())
print("Minimum result is: %02f" % (distance_matrix.min_cost() * 2 * cnt))

calculation = pymue.Calculation(cnt, distance_matrix, way_cost(MAX_WAY - 0.1))

print("calculate best routes....")


def generate_plan(calculation):
    plan = []
    for round_num in range(3):
        round_set = defaultdict(set)
        for (team_id, station) in enumerate(calculation.round_stations(pymue.Round(round_num))):
            round_set[station].add(team_id)
        plan.append(round_set.values())
    return plan



best_plan = None


def test():
    pymue.calculate(calculation)
    best_plan = generate_plan(calculation)
    print("")
    print("======best plan======")
    print("1st round:", best_plan[0])
    print("2nd round:", best_plan[1])
    print("3rd round:", best_plan[2])

test()
print("")
print("teams:", cnt)
print("solutions that where calculated:", calculation.solutions())
