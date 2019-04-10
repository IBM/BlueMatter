# utilities to carry out simple operations on graphs which are
# represented by dictionaries as adjacency lists
# {node_idA : [other_id1, other_id2, ..., other_idN], node_idB : ....

# depth first search returns a list comprising node ids traversed

class DepthFirst:
    UNSEEN = -1
    TOUCHED = 0
    VISITED = 1
    def __init__(self, graph):
        self.__graph = graph
        self.__visitStatus = {}
        self.__visitList = []
        for i in graph.keys():
            self.__visitStatus[i] = DepthFirst.UNSEEN
        touched = []
        self.visit(touched)
    def visitList(self):
        return self.__visitList
    def visit(self, touched):
        for i in self.__graph.keys():
            if self.__visitStatus[i] == DepthFirst.UNSEEN:
                self.__visitStatus[i] = DepthFirst.TOUCHED
                touched.append(i)
                print "--push-touched:", i, self.__visitStatus[i]
            while len(touched) > 0:
                current = touched.pop()
                print "--pop-touched:", current, self.__visitStatus[current]
                if self.__visitStatus[current] == DepthFirst.VISITED:
                    self.__visitList.append(current)
                    print "--append-visitList:", current
                    continue
                self.__visitStatus[current] = DepthFirst.VISITED
                touched.append(current)
                print "--push-touched (visited):", current, self.__visitStatus[current]
                for n in self.__graph[current]: # loop over links to current
                    if self.__visitStatus[n] == DepthFirst.UNSEEN:
                        self.__visitStatus[n] = DepthFirst.TOUCHED
                        touched.append(n)
                        print "--push-touched:", n, self.__visitStatus[n]
                
                    
        
    
