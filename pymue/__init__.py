from _pymue import peng, Team, DistanceMatrix, SeenTable, GuestTupleGenerator, GuestPair, Calculation

def pair_pprint(pair):
    return "(%s, %s)" % (pair.first, pair.second)

GuestPair.__repr__ = pair_pprint
