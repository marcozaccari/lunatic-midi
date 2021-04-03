package main

// #include <stdio.h>
// void hello() {  puts("Hello, Arch!"); }
import "C"

func main() {
	C.hello() 
}