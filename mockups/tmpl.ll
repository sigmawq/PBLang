; ModuleID = 'PBLANG JIT MODULE'
source_filename = "PBLANG JIT MODULE"

define i32 @foo() {
foo:
  ret double 4.900000e+01
}

define i32 @main() {
main:
  %calltmp = call i32 @foo()
  ret i32 %calltmp
}