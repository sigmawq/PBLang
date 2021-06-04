; ModuleID = 'PBLANG JIT MODULE'
source_filename = "PBLANG JIT MODULE"

@0 = private unnamed_addr constant [5 x i8] c"\22%d\22\00", align 1

define i32 @main() {
main:
  %a = alloca i32, align 8
  store i32 5, i32* %a, align 4
  %calltmp = call i32 @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @0, i32 0, i32 0), i32* %a)
  ret i32 0
}