

def minCoordinate(clist):
    if len(clist) < 2: return clist[0]
    try:
        x,  y  = clist[0]
        for x1, y1 in clist[1:]:
            if x1 <= x or y1 <= y:
                x, y = x1, y1
    except:
        x, y = 0, 0

    return x,y

def maxCoordinate(clist):
    if len(clist) < 2: return clist[0]
    try:
        x,  y  = clist[0]
        for x1, y1 in clist[1:]:
            if x1 >= x or y1 >= y:
                x, y = x1, y1
    except:
        x, y = 0, 0

    return x,y

def minBound(clist):
    x = 10000000
    y = 10000000
    for x1, y1 in clist:
        if x1 < x: x = x1
        if y1 < y: y = y1        
    return x,y

def maxBound(clist):
    x = -10000000
    y = -10000000
    for x1, y1 in clist:
        if x1 > x: x = x1
        if y1 > y: y = y1        
    return x,y

if __name__ == '__main__':

    tlist = [ (5,5), (3,6), (3,3), (-2,5), (100,100),
              (100,-100), (100,101) ]

    print minCoordinate(tlist)
    print maxCoordinate(tlist)
    print maxCoordinate([(3,3), (10,-1)])
