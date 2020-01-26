all: go-rubik

go-rubik: visualizer.go solver.go
	go build -o $@ -ldflags "-s -w" $^
fclean:
	rm -f go-rubik
re: fclean go-rubik
