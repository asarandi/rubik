package main

import (
	"container/list"
	"fmt"
	"time"
)

const (
	printPhaseMoves = true
)

type (
	cubeState struct {
		edges              [12]int
		edgeOrientations   [12]int
		corners            [8]int
		cornerOrientations [8]int
	}
)

var (
	moveNames = []string{
		"F", "F'", "F2",
		"B", "B'", "B2",
		"U", "U'", "U2",
		"D", "D'", "D2",
		"R", "R'", "R2",
		"L", "L'", "L2",
	}
	moveNameIdx = map[string]int{
		"F": 0, "F'": 1, "F2": 2,
		"B": 3, "B'": 4, "B2": 5,
		"U": 6, "U'": 7, "U2": 8,
		"D": 9, "D'": 10, "D2": 11,
		"R": 12, "R'": 13, "R2": 14,
		"L": 15, "L'": 16, "L2": 17,
	}
)

func phaseOneHash(c cubeState) int64 {
	var i int
	var res int64

	for i = 0; i < 12; i++ {
		res <<= 1
		res |= int64(c.edgeOrientations[i])
	}
	return res
}

func phaseTwoHash(c cubeState) int64 {
	var i int
	var res int64

	for i = 0; i < 8; i++ {
		res <<= 3
		res |= int64(c.cornerOrientations[i])
	}
	for i := 0; i < 12; i++ {
		res <<= 1
		if c.edges[i] > 7 {
			res |= 1
		}
	}
	return res
}

func phaseThreeHash(c cubeState) int64 {
	var (
		i, j int
		f, s [4]int
		res  int64
	)

	for i = 0; i < 12; i++ {
		res <<= 2
		if c.edges[i] > 7 {
			res |= 2
		} else {
			res |= int64(c.edges[i] & 1)
		}
	}
	for i = 0; i < 8; i++ {
		res <<= 3
		res |= int64(c.corners[i] & 5)
	}

	for i, j = 0, 0; i < 8; i++ {
		if c.corners[i] < 4 {
			f[c.corners[i]&3] = j
			j += 1
		} else {
			s[i-j] = c.corners[i] & 3
		}
	}
	res <<= 3
	res |= int64((f[s[0]] ^ f[s[1]]) << 1)
	if (f[s[0]] ^ f[s[2]]) > (f[s[0]] ^ f[s[3]]) {
		res |= 1
	}
	return res
}

func phaseFourHash(c cubeState) int64 {
	var res, prime uint64 = 0xcbf29ce484222325, 0x00000100000001B3
	var i int

	for i = 0; i < 12; i++ {
		res ^= uint64(c.edges[i])
		res *= prime
		res ^= uint64(c.edgeOrientations[i])
		res *= prime
		if i > 7 {
			continue
		}
		res ^= uint64(c.corners[i])
		res *= prime
		res ^= uint64(c.cornerOrientations[i])
		res *= prime
	}
	return int64(res)
}

func (c *cubeState) move(idx int) {
	var (
		rotations = [3][4]int{
			{1, 2, 3, 0}, // clockwise rotation: 0, 1, 2, 3 => 1, 2, 3, 0
			{3, 0, 1, 2}, // anti-clockwise
			{2, 3, 0, 1}, // twice
		}
		movingEdges = [6][4]int{
			{0, 9, 4, 8},   // F
			{2, 10, 6, 11}, // B
			{0, 1, 2, 3},   // U
			{4, 7, 6, 5},   // D
			{1, 8, 5, 10},  // R
			{3, 11, 7, 9},  // L
		}
		movingCorners = [6][4]int{
			{0, 3, 5, 4}, // F
			{2, 1, 7, 6}, // B
			{0, 1, 2, 3}, // U
			{4, 5, 6, 7}, // D
			{1, 0, 4, 7}, // R
			{3, 2, 6, 5}, // L
		}
		face, move = idx / 3, idx % 3
		i, j, k    int
		clone      = *c
	)

	for i = 0; i < 4; i++ {
		j = movingEdges[face][i]
		k = movingEdges[face][rotations[move][i]]
		c.edges[j] = clone.edges[k]
		c.edgeOrientations[j] = clone.edgeOrientations[k]
		if (face < 2) && (move < 2) {
			c.edgeOrientations[j] ^= 1 /* F,B half turns flip edge orientations */
		}
		j = movingCorners[face][i]
		k = movingCorners[face][rotations[move][i]]
		c.corners[j] = clone.corners[k]
		c.cornerOrientations[j] = clone.cornerOrientations[k]
		if (face != 2) && (face != 3) && (move < 2) { /* F,B,R,L half turns change corner orientations */
			c.cornerOrientations[j] = (c.cornerOrientations[j] + (2 - (i & 1))) % 3
		}
	}
}

func inverseMove(move int) int {
	if (move % 3) != 2 {
		move = move - (move % 3) + ((move % 3) ^ 1)
	}
	return move
}

func bfs(start cubeState, hf func(c cubeState) int64, moves []int) *list.List {
	var (
		finish                = solvedState()
		hashStart, hashFinish = hf(start), hf(finish)
		solution              = list.New()
	)

	if hashStart == hashFinish {
		return solution
	}

	var (
		node, child          cubeState
		queue                = list.New()
		parent               = make(map[int64]int64)
		direction            = make(map[int64]int)
		lastMove             = make(map[int64]int)
		hashNode, hashChild  int64
		i, dirNode, dirChild int
		ok                   bool
	)

	direction[hashStart] = 0
	direction[hashFinish] = 1
	queue.PushBack(start)
	queue.PushBack(finish)
	for queue.Len() > 0 {
		node = queue.Remove(queue.Front()).(cubeState)
		hashNode = hf(node)
		dirNode = direction[hashNode]
		for i = 0; i < 18; i++ {
			if moves[i] == 0 {
				continue
			}
			child = node
			child.move(i)
			hashChild = hf(child)
			if dirChild, ok = direction[hashChild]; ok && dirChild != dirNode {
				if dirNode == 1 {
					hashChild, hashNode, i = hashNode, hashChild, inverseMove(i)
				}
				solution.PushFront(i)
				for hashNode != hashStart {
					solution.PushFront(lastMove[hashNode])
					hashNode = parent[hashNode]
				}
				for hashChild != hashFinish {
					solution.PushBack(inverseMove(lastMove[hashChild]))
					hashChild = parent[hashChild]
				}
				return solution
			}
			if _, ok = direction[hashChild]; !ok {
				direction[hashChild] = dirNode
			}
			if _, ok = lastMove[hashChild]; !ok {
				lastMove[hashChild] = i
			}
			if _, ok = parent[hashChild]; !ok {
				parent[hashChild] = hashNode
			}
			queue.PushBack(child)
		}
	}
	return solution
}

func solvedState() cubeState {
	var c cubeState
	var i int

	for i = 0; i < 12; i++ {
		c.edges[i] = i
		c.edgeOrientations[i] = 0
	}
	for i = 0; i < 8; i++ {
		c.corners[i] = i
		c.cornerOrientations[i] = 0
	}
	return c
}

const pfLeft = 25

func printCubeConfig(c cubeState) {
	var edgeNames = [][]string{
		{"UF", "UR", "UB", "UL", "DF", "DR", "DB", "DL", "FR", "FL", "BR", "BL"},
		{"FU", "RU", "BU", "LU", "FD", "RD", "BD", "LD", "RF", "LF", "RB", "LB"},
	}
	var cornerNames = [][]string{
		{"UFR", "URB", "UBL", "ULF", "DRF", "DFL", "DLB", "DBR"},
		{"RUF", "BUR", "LUB", "FUL", "FDR", "LDF", "BDL", "RDB"},
		{"FRU", "RBU", "BLU", "LFU", "RFD", "FLD", "LBD", "BRD"},
	}
	var i int

	fmt.Printf("%*s:\t ", pfLeft, "edge/corner positions")
	for i = 0; i < 12; i++ {
		fmt.Printf("%2d ", c.edges[i])
	}
	fmt.Printf("    ")
	for i = 0; i < 8; i++ {
		fmt.Printf("%3d ", c.corners[i])
	}
	fmt.Printf("\n")

	fmt.Printf("%*s:\t ", pfLeft, "edge/corner orientations")
	for i = 0; i < 12; i++ {
		fmt.Printf("%2d ", c.edgeOrientations[i])
	}
	fmt.Printf("    ")
	for i = 0; i < 8; i++ {
		fmt.Printf("%3d ", c.cornerOrientations[i])
	}
	fmt.Printf("\n")

	fmt.Printf("%*s:\t ", pfLeft, "michael reid notation")
	for i = 0; i < 12; i++ {
		fmt.Printf("%s ", edgeNames[c.edgeOrientations[i]][c.edges[i]])
	}
	fmt.Printf("    ")
	for i = 0; i < 8; i++ {
		fmt.Printf("%s ", cornerNames[c.cornerOrientations[i]][c.corners[i]])
	}
	fmt.Printf("\n\n")
}

func solve(c cubeState) *list.List {
	var (
		hashes = []func(c cubeState) int64{phaseOneHash, phaseTwoHash, phaseThreeHash, phaseFourHash}
		moves  = [][]int{
			{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //all moves
			{0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //all except F,F',B,B'
			{0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1}, //all except F,F',B,B',R,R',L,L'
			{0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1}, //only F2,B2,U2,D2,R2,L2 moves
		}
		solution    = list.New()
		clone       = c
		steps       *list.List
		i, numSteps int
		t           int64
	)

	t = time.Now().UnixNano()
	for i, numSteps = 0, 0; i < 4; i++ {
		steps = bfs(clone, hashes[i], moves[i])
		if printPhaseMoves && steps.Len() > 0 {
			fmt.Printf("%*s %d:\t ", pfLeft-2, "thistlethwaite phase", i+1)
		}
		for steps.Len() > 0 {
			move := steps.Remove(steps.Front()).(int)
			solution.PushBack(move)
			clone.move(move)
			numSteps += 1
			if printPhaseMoves {
				fmt.Printf("%s ", moveNames[move])
				if steps.Len() == 0 {
					fmt.Printf("\n")
				}
			}
		}
	}
	t = time.Now().UnixNano() - t
	if printPhaseMoves && solution.Len() > 0 {
		fmt.Printf("%*s:\t %.4f second(s)\n", pfLeft, "search duration", float64(t)/1000000000.0)
		fmt.Printf("%*s:\t %d moves\n\n", pfLeft, "solution length", numSteps)
	}
	return solution
}
