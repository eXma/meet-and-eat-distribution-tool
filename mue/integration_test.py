from random import choice
import json
import sys
import pymue
from collections import defaultdict

MAX_WAY = 7.8


def way_cost(way_length):
    if way_length <= 1:
        return way_length * 100
    elif way_length < MAX_WAY:
        return (way_length * 100) ** 2
    return sys.float_info.max


print "read data...."

pref = ""
if len(sys.argv):
    pref = "_%s" % sys.argv[1]

with open("teams%s.json" % pref, "r") as f:
    team_data = json.load(f)

with open("distances%s.json" % pref, "r") as f:
    distance_data = json.load(f)

print "map teams...."
team_map = dict()
team_map_reverse = dict()
for (idx, team) in enumerate(team_data):
    team_map[team["id"]] = idx
    team_map_reverse[idx] = team["id"]

cnt = len(team_data)
teams = sorted(team_map.values())

print "build distance matrix..."
distance_matrix = pymue.DistanceMatrix(cnt)
for distance_run in distance_data:
    for src in distance_run:
        for dst in distance_run[src]:
            distance_matrix.set_cost(team_map[int(src)],
                                     team_map[int(dst)],
                                     way_cost(distance_run[src][dst]))

calculation = pymue.Calculation(cnt, distance_matrix)

print "calculate best routes...."

cnt_hosts = cnt / 3



def generate_plan(round_data, iteration_data):
    plan = []
    for round_num in range(3):
        round_set = defaultdict(set)
        for (team_id, station) in enumerate(calculation.round_stations(pymue.Round(round_num), round_data, iteration_data)):
            round_set[station].add(team_id)
        plan.append(round_set.values())
    return plan



i = 0
best_distance = sys.float_info.max
best_plan = None



def deploy_host(host_idx, current_round, iteration_data, round_data):
    """
    :param seen_table: dict of set
    """
    if host_idx == cnt_hosts:
        if current_round < 2:
            new_round = current_round + 1
            round_data = calculation.next_round_data(round_data, iteration_data)
            iteration_data.clear_round_data()
            return deploy_host(0, new_round, iteration_data, round_data)
        else:
            global i, best_distance, best_plan
            i += 1
            if iteration_data.distance < best_distance:
                best_plan = generate_plan(round_data, iteration_data)
                print "new best (%i)" % i, iteration_data.distance, best_plan
                best_distance = iteration_data.distance
                calculation.update_best(best_distance)
            return

    tests = cnt_hosts * 3
    actual_host = round_data.hosts[host_idx]
    if current_round > 0:
        iteration_data.distance = calculation.host_distance(round_data, actual_host) + iteration_data.distance
        if iteration_data.distance >= best_distance:
            return

    possible_guests = calculation.determine_guest_candidates(round_data, iteration_data, actual_host)


    if current_round > 0:
        tests = cnt_hosts / 3

    for candidate in possible_guests[:tests]:
        if current_round == 0:
            actual_distance = iteration_data.distance
        else:
            actual_distance = candidate.distance
        guests = candidate.guests

        new_iteration_data = pymue.IterationData(iteration_data)
        new_iteration_data.distance = actual_distance
        new_iteration_data.set_station(actual_host, guests.first, guests.second)

        deploy_host(host_idx + 1, current_round, new_iteration_data, round_data)


def test():
    round_data = calculation.initial_round_data()
    iteration_data = pymue.IterationData(cnt)
    deploy_host(0, 0, iteration_data, round_data)
    print ""
    print "======best plan======"
    print "1st round:", best_plan[0]
    print "2nd round:", best_plan[1]
    print "3rd round:", best_plan[2]

test()
print ""
print "teams:", cnt
print "solutions that where calculated:", i
