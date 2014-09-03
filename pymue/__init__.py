from _pymue import Team, DistanceMatrix, SeenTable, GuestTupleGenerator, GuestPair, Calculation, RoundData, IterationData, GuestCandidate

def pair_pprint(pair):
    return "(%s, %s)" % (pair.first, pair.second)

GuestPair.__repr__ = pair_pprint
