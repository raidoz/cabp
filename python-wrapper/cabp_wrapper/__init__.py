# Basic wrapper around the CABP CPP implementation from 2009.
# author Raido Pahtma

# CABP is in CPP with a C proxy around it
import ctypes

# UTM coordinate transformation https://github.com/Turbo87/utm
import utm

cabp = ctypes.CDLL("libCABP.so")

def cabp_position_local(nodes):
    pos_func = cabp.cabp_position
    pos_func.argtypes = [ctypes.POINTER(ctypes.c_int16), ctypes.POINTER(ctypes.c_int16), ctypes.POINTER(ctypes.c_uint16)]
    pos_func.restype = ctypes.c_int

    x = ctypes.c_int16()
    y = ctypes.c_int16()
    r = ctypes.c_uint16()

    for node in nodes:
        cabp.cabp_add_comm_node(node[0], int(node[1]), int(node[2]), node[3])

    result = cabp.cabp_position(x, y, r)

    cabp.cabp_clear()

    print("result {}: {} {} {}".format(result, x, y, r))

    return (x.value, y.value, r.value)

def cabp_position_global(nodes):
    zone = None # Zone tuple

    min_east = 1000000 # maximum easting from the utm library
    min_north = 10000000 # maximum northing from the utm library

    # Convert all coordinates to UTM, make sure only one zone is used
    # Situations where nodes span multiple zones do not currently work
    nodes_utm = []
    for node in nodes:
        east, north, zn, zl = utm.from_latlon(node[1], node[2])
        if zone is None:
            zone = (zn, zl)

        if east < min_east:
            min_east = east

        if north < min_north:
            min_north = north

        if (zn, zl) == zone:
            print("UTM: {} ({};{})".format(node[0], east, north))
            nodes_utm.append((node[0], east, north, node[3]))
        else:
            print("WARN: zone mismatch, discarding node {}", node[0])

    # Bring all coordinates relative to the bottom-left because CABP library does
    # not deal well with big numbers (int16 for coordinates)
    # Thing probably also break if nodes spaced very wide!
    nodes_botlef = []
    for node in nodes_utm:
        nodes_botlef.append((node[0], node[1]-min_east, node[2]-min_north, node[3]))

    if len(nodes_botlef) > 0:
        # Calculate positiong in the bottom-left system
        x, y, r = cabp_position_local(nodes_botlef)

        # Convert to UTM by adding the offsets, convert to latitude-longitude
        lat, lon = utm.to_latlon(x+min_east, y+min_north, zone[0], zone[1], strict=False)

        return (lat, lon, r)

    return None
