package main

import (
	"container/list"
)

type cubeState struct {
	edges              [12]int
	edgeOrientations   [12]int
	corners            [8]int
	cornerOrientations [8]int
}

func solvedState() cubeState {
	c := cubeState{}
	for i := 0; i < 12; i++ {
		c.edges[i] = i
	}
	for i := 0; i < 8; i++ {
		c.corners[i] = i
	}
	return c
}

var moveNames = []string{
	"F", "F'", "F2",
	"B", "B'", "B2",
	"U", "U'", "U2",
	"D", "D'", "D2",
	"R", "R'", "R2",
	"L", "L'", "L2",
}

var moveNameIdx = map[string]int{
	"F": 0, "F'": 1, "F2": 2,
	"B": 3, "B'": 4, "B2": 5,
	"U": 6, "U'": 7, "U2": 8,
	"D": 9, "D'": 10, "D2": 11,
	"R": 12, "R'": 13, "R2": 14,
	"L": 15, "L'": 16, "L2": 17,
}
var phaseOneMoves = []int{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}   //all moves
var phaseTwoMoves = []int{0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}   //all except F,F',B,B'
var phaseThreeMoves = []int{0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1} //all except F,F',B,B',R,R',L,L'
var phaseFourMoves = []int{0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1}  //only F2,B2,U2,D2,R2,L2 moves

func phaseOneHash(c cubeState) int64 {
	var res int64
	for i := 0; i < 12; i++ {
		res <<= 1
		res |= int64(c.edgeOrientations[i])
	}
	return res
}

func phaseTwoHash(c cubeState) int64 {
	var res int64
	for i := 0; i < 8; i++ {
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
	var res int64
	var f [4]int
	var s [4]int

	for i := 0; i < 12; i++ {
		res <<= 2
		if c.edges[i] > 7 {
			res |= 2
		} else {
			res |= int64(c.edges[i] & 1)
		}
	}
	for i := 0; i < 8; i++ {
		res <<= 3
		res |= int64(c.corners[i] & 5)
	}

	for i, j := 0, 0; i < 8; i++ {
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
	var res uint64 = 0xcbf29ce484222325
	var prime uint64 = 0x00000100000001B3
	for i := 0; i < 12; i++ {
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
	face := idx / 3
	move := idx % 3
	clone := *c
	rotations := [3][4]int{
		{1, 2, 3, 0}, // clockwise rotation: 0, 1, 2, 3 => 1, 2, 3, 0
		{3, 0, 1, 2}, // anti-clockwise
		{2, 3, 0, 1}, // twice
	}
	movingEdges := [6][4]int{
		{0, 9, 4, 8},   // F
		{2, 10, 6, 11}, // B
		{0, 1, 2, 3},   // U
		{4, 7, 6, 5},   // D
		{1, 8, 5, 10},  // R
		{3, 11, 7, 9},  // L
	}
	movingCorners := [6][4]int{
		{0, 3, 5, 4}, // F
		{2, 1, 7, 6}, // B
		{0, 1, 2, 3}, // U
		{4, 5, 6, 7}, // D
		{1, 0, 4, 7}, // R
		{3, 2, 6, 5}, // L
	}
	for i := 0; i < 4; i++ {
		j := movingEdges[face][i]
		k := movingEdges[face][rotations[move][i]]
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
	finish := solvedState()
	hashStart := hf(start)
	hashFinish := hf(finish)
	solution := list.New()
	if hashStart == hashFinish {
		return solution
	}
	direction := make(map[int64]int)
	lastMove := make(map[int64]int)
	parent := make(map[int64]int64)
	direction[hashStart] = 0
	direction[hashFinish] = 1
	queue := list.New()
	queue.PushBack(start)
	queue.PushBack(finish)
	for queue.Len() > 0 {
		node := queue.Remove(queue.Front()).(cubeState)
		hashNode := hf(node)
		dirNode := direction[hashNode]
		for i := 0; i < 18; i++ {
			if moves[i] == 0 {
				continue
			}
			child := node
			child.move(i)
			hashChild := hf(child)
			if dirChild, ok := direction[hashChild]; ok && dirChild != dirNode {
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
			if _, ok := direction[hashChild]; !ok {
				direction[hashChild] = dirNode
			}
			if _, ok := lastMove[hashChild]; !ok {
				lastMove[hashChild] = i
			}
			if _, ok := parent[hashChild]; !ok {
				parent[hashChild] = hashNode
			}
			queue.PushBack(child)
		}
	}
	return solution
}

func (c *cubeState) solve() *list.List {
	var q *list.List

	hashes := []func(c cubeState) int64{phaseOneHash, phaseTwoHash, phaseThreeHash, phaseFourHash}
	moves := [][]int{phaseOneMoves, phaseTwoMoves, phaseThreeMoves, phaseFourMoves}

	solution := list.New()
	clone := *c

	for i := 0; i < 4; i++ {
		q = bfs(clone, hashes[i], moves[i])
		for q.Len() > 0 {
			move := q.Remove(q.Front()).(int)
			solution.PushBack(move)
			clone.move(move)
		}
	}
	return solution
}
