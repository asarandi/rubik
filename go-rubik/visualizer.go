package main

import (
	"container/list"
	"fmt"
	_ "fmt"
	"github.com/go-gl/gl/v2.1/gl"
	"github.com/veandco/go-sdl2/sdl"
	"math"
	"os"
	"strings"
	"time"
)

var cube cubeState
var queue *list.List
var winWidth, winHeight int32 = 640, 480

var moveIndex = -1
var moveStartedTime int64
var running bool
var perspectiveX float32 = 35.264
var perspectiveY float32 = -45.0

const moveDuration = 250000000
const oneDegree = -0.0174533
const perspectiveUnit = 5.625

var cubeColors = [6][3]float32{
	{0.00, 0.61, 0.28}, //F - green
	{0.00, 0.27, 0.68}, //B - blue
	{1.00, 1.00, 1.00}, //U - white
	{1.00, 0.84, 0.00}, //D - yellow
	{0.72, 0.07, 0.20}, //R - red
	{1.00, 0.35, 0.00}, //L - orange
}

type tCubeFace struct {
	v bool
	c [4][3]float64
}
type tCube [6]tCubeFace

var cubeArray []tCube
var cubeArrayCopy []tCube

var facesArray = [6][9]int{
	{6, 15, 24, 3, 12, 21, 0, 9, 18},     // 0 F green
	{26, 17, 8, 23, 14, 5, 20, 11, 2},    // 1 B blue
	{8, 17, 26, 7, 16, 25, 6, 15, 24},    // 2 U white
	{0, 9, 18, 1, 10, 19, 2, 11, 20},     // 3 D yellow
	{24, 25, 26, 21, 22, 23, 18, 19, 20}, // 4 R red
	{8, 7, 6, 5, 4, 3, 2, 1, 0},          // 5 L orange
}

var facesArrayCopy = [6][9]int{
	{6, 15, 24, 3, 12, 21, 0, 9, 18},     // 0 F green
	{26, 17, 8, 23, 14, 5, 20, 11, 2},    // 1 B blue
	{8, 17, 26, 7, 16, 25, 6, 15, 24},    // 2 U white
	{0, 9, 18, 1, 10, 19, 2, 11, 20},     // 3 D yellow
	{24, 25, 26, 21, 22, 23, 18, 19, 20}, // 4 R red
	{8, 7, 6, 5, 4, 3, 2, 1, 0},          // 5 L orange
}

func (t *tCube) rotateX(angle float64) {
	for i := 0; i < 6; i++ {
		for j := 0; j < 4; j++ {
			y, z := t[i].c[j][1], t[i].c[j][2]
			t[i].c[j][1] = (y * math.Cos(angle)) - (z * math.Sin(angle))
			t[i].c[j][2] = (y * math.Sin(angle)) + (z * math.Cos(angle))
		}
	}
}

func (t *tCube) rotateY(angle float64) {
	for i := 0; i < 6; i++ {
		for j := 0; j < 4; j++ {
			x, z := t[i].c[j][0], t[i].c[j][2]
			t[i].c[j][0] = (x * math.Cos(angle)) + (z * math.Sin(angle))
			t[i].c[j][2] = (z * math.Cos(angle)) - (x * math.Sin(angle))
		}
	}
}

func (t *tCube) rotateZ(angle float64) {
	for i := 0; i < 6; i++ {
		for j := 0; j < 4; j++ {
			x, y := t[i].c[j][0], t[i].c[j][1]
			t[i].c[j][0] = (x * math.Cos(angle)) - (y * math.Sin(angle))
			t[i].c[j][1] = (x * math.Sin(angle)) + (y * math.Cos(angle))
		}
	}
}

func rotateFaceFloats(f int, angle float64) {
	var i, j int
	var rot = []func(t *tCube, a float64){
		func(t *tCube, a float64) { t.rotateZ(a) },
		func(t *tCube, a float64) { t.rotateY(a) },
		func(t *tCube, a float64) { t.rotateX(a) },
	}
	for i = 0; i < 9; i++ {
		j = facesArray[f][i]
		rot[f/2](&cubeArray[j], angle)
	}
}

func updateFacesArray(move int) {
	var i, j, k, f int

	f = move / 3
	var rotations = [3][9]int{
		{6, 3, 0, 7, 4, 1, 8, 5, 2}, //clockwise
		{2, 5, 8, 1, 4, 7, 0, 3, 6}, //anti-clockwise
		{8, 7, 6, 5, 4, 3, 2, 1, 0}, //twice
	}
	clone := facesArray[f]
	for i := 0; i < 9; i++ {
		facesArray[f][i] = clone[rotations[move%3][i]]
	}

	for i = 0; i < 6; i++ {
		if i == f {
			continue
		}
		for j = 0; j < 9; j++ {
			for k = 0; k < 9; k++ {
				if facesArrayCopy[i][j] == facesArrayCopy[f][k] {
					facesArray[i][j] = facesArray[f][k]
				}
			}
		}
	}

}

func (t *tCube) initCube(x, y, z float64) {
	var coords = [6][4][3]float64{
		{{0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0},},
		{{0, 0, -1}, {0, 1, -1}, {1, 1, -1}, {1, 0, -1},},
		{{0, 1, 0}, {0, 1, -1}, {1, 1, -1}, {1, 1, 0},},
		{{0, 0, 0}, {0, 0, -1}, {1, 0, -1}, {1, 0, 0},},
		{{1, 0, 0}, {1, 1, 0}, {1, 1, -1}, {1, 0, -1},},
		{{0, 0, 0}, {0, 1, 0}, {0, 1, -1}, {0, 0, -1},},
	}
	var f [3]float64
	var u = 2.0
	var i, j, k int

	f[0], f[1], f[2] = x, y, z
	have := [6]float64{z, z, y, y, x, x}
	want := [6]float64{3, -1, 1, -3, 1, -3}
	for i = 0; i < 6; i++ {
		t[i].v = want[i] == have[i]
		for j = 0; j < 4; j++ {
			for k = 0; k < 3; k++ {
				t[i].c[j][k] = f[k] + coords[i][j][k]*u
			}
		}
	}
}

func initAllCubes() {
	var idx, i, j, k int
	cubeArray, cubeArrayCopy = make([]tCube, 27), make([]tCube, 27)
	for i = -3; i < 3; i += 2 {
		for j = -3; j < 3; j += 2 {
			for k = 3; k > -3; k -= 2 {
				cubeArray[idx].initCube(float64(i), float64(j), float64(k))
				cubeArrayCopy[idx].initCube(float64(i), float64(j), float64(k))
				idx += 1
			}
		}
	}
}

func drawGlScene() {
	var t = [8]int{0, 1, 2, 3, 0, 3, 1, 2}
	var i, j, k int

	gl.ClearColor(0.3, 0.3, 0.3, 0.0)
	gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)
	gl.LoadIdentity()
	gl.Translatef(0.0, 0.0, -18.0)
	gl.Rotatef(perspectiveX, 1.0, 0.0, 0.0)
	gl.Rotatef(perspectiveY, 0.0, 1.0, 0.0)
	gl.Begin(gl.QUADS)
	for i = 0; i < 27; i++ {
		for j = 0; j < 6; j++ {
			if cubeArray[i][j].v {
				gl.Color4f(cubeColors[j][0], cubeColors[j][1], cubeColors[j][2], 1.0)
			} else {
				gl.Color4f(0.08, 0.08, 0.08, 1.0)
			}
			for k = 0; k < 4; k++ {
				gl.Vertex3d(cubeArray[i][j].c[k][0], cubeArray[i][j].c[k][1], cubeArray[i][j].c[k][2])
			}
		}
	}
	gl.End()
	//gl.Enable(gl.BLEND)
	//gl.BlendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA)
	gl.LineWidth(4.0)
	gl.Begin(gl.LINES)
	gl.Color4f(0.08, 0.08, 0.08, 0.5)
	for i = 0; i < 27; i++ {
		for j = 0; j < 8; j++ {
			gl.Vertex3d(cubeArray[i][0].c[t[j]][0], cubeArray[i][0].c[t[j]][1], cubeArray[i][0].c[t[j]][2])
		}
		for j = 0; j < 8; j++ {
			gl.Vertex3d(cubeArray[i][1].c[t[j]][0], cubeArray[i][1].c[t[j]][1], cubeArray[i][1].c[t[j]][2])
		}
		for j = 0; j < 4; j++ {
			gl.Vertex3d(cubeArray[i][0].c[j][0], cubeArray[i][0].c[j][1], cubeArray[i][0].c[j][2])
			gl.Vertex3d(cubeArray[i][1].c[j][0], cubeArray[i][1].c[j][1], cubeArray[i][1].c[j][2])
		}
	}
	gl.End()
	gl.Disable(gl.BLEND)
}

func initGl() {
	//gl.ShadeModel(gl.SMOOTH)
	gl.ClearColor(0.0, 0.0, 0.0, 0.0)
	gl.ClearDepth(1.0)
	gl.Enable(gl.DEPTH_TEST)
	gl.DepthFunc(gl.LEQUAL)
	//gl.Hint(gl.PERSPECTIVE_CORRECTION_HINT, gl.NICEST)
	gl.Viewport(0, 0, winWidth, winHeight)
	gl.MatrixMode(gl.PROJECTION)
	gl.LoadIdentity()
	fH := math.Tan(45.0/360.0*math.Pi) * 0.1
	fW := fH * float64(winWidth) / float64(winHeight)
	gl.Frustum(-fW, fW, -fH, fH, 0.1, 100)
	gl.MatrixMode(gl.MODELVIEW)
	gl.LoadIdentity()
}

func parseArgs() {
	for _, arg := range os.Args[1:] {
		split := strings.Split(arg, " ")
		for _, str := range split {
			str = strings.ToUpper(strings.Trim(str, " \t\r\n\f\v"))
			if move, ok := moveNameIdx[str]; ok {
				queue.PushBack(move)
			}
		}
	}
}

func processMoves() {
	moveDirections := []float64{1, -1, 2, -1, 1, -2}
	if moveIndex == -1 {
		if queue.Len() > 0 {
			moveIndex = queue.Remove(queue.Front()).(int)
			moveStartedTime = time.Now().UnixNano()
			copy(cubeArrayCopy, cubeArray)
		}
		return
	}
	t := time.Now().UnixNano()
	copy(cubeArray, cubeArrayCopy)
	d := moveDirections[moveIndex%6]
	if t >= moveStartedTime+moveDuration {
		rotateFaceFloats(moveIndex/3, d*90.0*oneDegree)
		updateFacesArray(moveIndex)
		cube.move(moveIndex)
		moveIndex = -1
		return
	}
	p := math.Round(float64(t-moveStartedTime) / float64(moveDuration/100))
	k := math.Round((math.Abs(d) * 90.0) / 100.0 * p)
	if d < 0 {
		k = -k
	}
	rotateFaceFloats(moveIndex/3, k*oneDegree)
}

func handleKeyPress(t *sdl.KeyboardEvent) {
	keys := []sdl.Keycode{
		sdl.K_f, sdl.K_f, sdl.K_f,
		sdl.K_b, sdl.K_b, sdl.K_b,
		sdl.K_u, sdl.K_u, sdl.K_u,
		sdl.K_d, sdl.K_d, sdl.K_d,
		sdl.K_r, sdl.K_r, sdl.K_r,
		sdl.K_l, sdl.K_l, sdl.K_l,
	}
	mods := []uint16{
		sdl.KMOD_NONE, sdl.KMOD_LCTRL, sdl.KMOD_LALT,
		sdl.KMOD_NONE, sdl.KMOD_LCTRL, sdl.KMOD_LALT,
		sdl.KMOD_NONE, sdl.KMOD_LCTRL, sdl.KMOD_LALT,
		sdl.KMOD_NONE, sdl.KMOD_LCTRL, sdl.KMOD_LALT,
		sdl.KMOD_NONE, sdl.KMOD_LCTRL, sdl.KMOD_LALT,
		sdl.KMOD_NONE, sdl.KMOD_LCTRL, sdl.KMOD_LALT,
	}
	for i := 0; t.Repeat == 0 && i < 18; i++ {
		if t.Keysym.Sym == keys[i] && t.Keysym.Mod == mods[i] {
			queue.PushBack(i)
			return
		}
	}

	switch t.Keysym.Sym {
	case sdl.K_RIGHT:
		perspectiveY += perspectiveUnit
	case sdl.K_LEFT:
		perspectiveY -= perspectiveUnit
	case sdl.K_UP:
		perspectiveX += perspectiveUnit
	case sdl.K_DOWN:
		perspectiveX -= perspectiveUnit
	case sdl.K_s:
		if t.Repeat != 0 || queue.Len() != 0 {
			break
		}
		solution := cube.solve()
		fmt.Printf("steps:  ")
		for solution.Len() > 0 {
			move := solution.Remove(solution.Front()).(int)
			queue.PushBack(move)
			fmt.Printf("%s ", moveNames[move])
		}
		fmt.Printf("\n")
	}
}

func main() {
	var winTitle = "go-rubik"
	var window *sdl.Window
	var context sdl.GLContext
	var event sdl.Event
	var err error

	if err = sdl.Init(sdl.INIT_EVERYTHING); err != nil {
		panic(err)
	}
	defer sdl.Quit()

	window, err = sdl.CreateWindow(winTitle, sdl.WINDOWPOS_CENTERED, sdl.WINDOWPOS_CENTERED,
		winWidth, winHeight, sdl.WINDOW_OPENGL)
	if err != nil {
		panic(err)
	}
	defer window.Destroy()
	context, err = window.GLCreateContext()
	if err != nil {
		panic(err)
	}
	defer sdl.GLDeleteContext(context)

	if err = gl.Init(); err != nil {
		panic(err)
	}

	initGl()
	initAllCubes()
	cube = solvedState()
	queue = list.New()
	parseArgs()

	running = true
	for running {
		for event = sdl.PollEvent(); event != nil; event = sdl.PollEvent() {
			switch t := event.(type) {
			case *sdl.QuitEvent:
				running = false
			case *sdl.KeyboardEvent:
				if t.State != sdl.PRESSED {
					break
				}
				keyCode := t.Keysym.Sym
				if keyCode == sdl.K_ESCAPE || keyCode == sdl.K_q {
					running = false
				}
				handleKeyPress(t)
			}
		}
		processMoves()
		drawGlScene()
		window.GLSwap()
	}
}