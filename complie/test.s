.text
.data
.text
.align 2
.global main
.syntax unified
.arm
.fpu vfp
.type main,%function
main:
push {fp,lr}
add fp,sp,#4
sub sp,sp,#4
mov r0, #0
sub sp,fp,#4
pop {fp,pc}
.L4:
.align 2
.L3:
