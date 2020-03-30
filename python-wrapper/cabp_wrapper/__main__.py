
from . import cabp_position_local, cabp_position_global

if __name__ == "__main__":
    nodes = [
        (1,100,100,54),
        (2,100,150,55),
        (3,150,100,53),
        (6,250,100,71),
        (7,250,150,60),
        (8,250,150,55)
    ]
    print("{}".format(cabp_position_local(nodes)))

    print("----------")

    nodes = [
        (1,59.768,29.124,100),
        (2,59.768,29.123,100),
    ]
    print("{}".format(cabp_position_global(nodes)))

    print("----------")

    nodes = []
    print("{}".format(cabp_position_global(nodes)))
