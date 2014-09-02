from random import choice
import json
import sys
import pymue

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


seen = pymue.SeenTable(cnt)


def round_hosts(round_num=0):
    begin = cnt_hosts * round_num
    end = begin + cnt_hosts
    return teams[begin:end]


def round_guests(hosts):
    """
    :param hosts: list of int
    """
    return [team for team in teams if team not in hosts]


def add_meeting(host, guests, seen_table, meetings, stations):
    tbl = seen_table.clone()
    tbl.add_meeting(host, guests.first, guests.second)

    meeting = {host, guests.first, guests.second}
    meetings.append(meeting)
    for team in [host, guests.first, guests.second]:
        stations[team] = host
    return tbl


def distance(current_distance, prev_stations, teams, host):
    for team in teams:
        prev_host = prev_stations[team]
        dist = distance_matrix.lookup(prev_host, host)
        current_distance += dist
    return current_distance


second_hosts_list = round_hosts(1)
second_hosts_set = set(second_hosts_list)


i = 0
best_distance = sys.float_info.max
best_plan = None

def guestsort(a, b):
    if a[0] < b[0]:
        return -1
    elif a[0] == b[0]:
        return 0
    else:
        return 1


def determine_guests(current_guests, used_guests, actual_host, seen_table, current_round, current_distance, last_stations):
    possible_guests = []
    round_data = pymue.RoundData(current_round, [], current_guests, [last_stations[i] for i in sorted(last_stations.keys())])
    for guests in pymue.GuestTupleGenerator(current_guests, used_guests):
        if not seen_table.seen(actual_host, guests.first, guests.second):
            if current_round > 0:
                actual_distance = calculation.guest_distance(round_data, actual_host, guests) + current_distance
                if actual_distance >= best_distance:
                    continue
            else:
                actual_distance = calculation.dummy_distance(actual_host, guests)
            possible_guests.append((actual_distance, guests))


    possible_guests.sort(guestsort)
    return possible_guests;


def deploy_host(host_idx, current_hosts, current_round, current_guests, used_guests, seen_table, meetings_list,
                stations_list, current_distance):
    """
    :param seen_table: dict of set
    """
    if host_idx == len(current_hosts):
        if current_round < 2:
            new_round = current_round + 1
            new_hosts = round_hosts(new_round)
            new_guests = round_guests(new_hosts)
            return deploy_host(0, new_hosts, new_round, new_guests, list(), seen_table, meetings_list, stations_list,
                               current_distance)
        else:
            global i, best_distance, best_plan
            i += 1
            #print "ENDPOINT", meetings_list
            if current_distance < best_distance:
                print "new best (%i)" % i, current_distance, meetings_list
                best_distance = current_distance
                best_plan = meetings_list
            return

    tests = cnt_hosts * 3
    actual_host = current_hosts[host_idx]
    if current_round > 0:
        current_distance = distance(current_distance, stations_list[current_round - 1],
                                    [actual_host], actual_host)
        if current_distance >= best_distance:
            #print "skip combination in round", current_round, "for host dist", current_distance, "best", best_distance
            return

    possible_guests = determine_guests(current_guests, used_guests, actual_host, seen_table, current_round, current_distance, stations_list[current_round - 1])


    if current_round > 0:
        tests = cnt_hosts / 3

    for (actual_distance, guests) in possible_guests[:tests]:
        if current_round == 0:
            actual_distance = current_distance

        actual_meetings = list(meetings_list) # XXX copy.deepcopy(meetings)
        meetings = list(actual_meetings[current_round])
        actual_meetings[current_round] = meetings

        actual_stations = list(stations_list)
        stations = actual_stations[current_round].copy()
        actual_stations[current_round] = stations

        actual_seen = add_meeting(actual_host, guests, seen_table, meetings, stations)
        actual_used = list(used_guests)
        actual_used.extend([guests.first, guests.second])
        deploy_host(host_idx + 1, current_hosts, current_round, current_guests, actual_used, actual_seen,
                    actual_meetings, actual_stations, actual_distance)


def test():
    current_hosts = round_hosts(0)
    current_guests = round_guests(current_hosts)

    deploy_host(0, current_hosts, 0, current_guests, [], seen, [[], [], []], [{}, {}, {}], 0)
    print ""
    print "======best plan======"
    print "1st round:", best_plan[0]
    print "2nd round:", best_plan[1]
    print "3rd round:", best_plan[2]

test()
print ""
print "teams:", cnt
print "solutions that where calculated:", i
