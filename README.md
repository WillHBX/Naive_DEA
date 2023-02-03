**DON'T FORGET TO CHANGE THE PARAMETER IN *main.c*  **
<br>

## FLOW CHART

```flow
st=>start: start
xy=>subroutine: readXY()
o=>inputoutput: output
optimizer=>subroutine: optimaizer()
e=>end: end

st(right)->xy(right)->optimizer(right)->o(right)->e
```
<br>

----

<br>
```flow
st=>start: readXY()
input=>inputoutput: readfile
ss=>operation: split & store
eq=>operation: initialize the
equal constraint
negX=>operation: change X
into -X
e=>end: return

st(right)->input(right)->ss(right)->eq(right)->negX(right)->e
```
<br>

----

<br>
```flow
st=>start: optimizer()
nm=>subroutine: new_model()
initC=>subroutine: init_constraints()
cond=>condition: list == NULL ?
opA=>subroutine: optimize_all()
opP=>subroutine: optimize_partOf()
e=>end: return
st(right)->nm(right)->initC(right)->cond
cond(yes,right)->opA(right)->e
cond(no)->opP(right)--->e
```
<br>

----

<br>
```flow
st=>start: new_Model()
create=>operation: Create an\
empty model 
para=>operation: set model parameter
cof=>operation: set the coeffients 
of the objective matrix
sence=>operation: Set objective sense to 'minimization'
e=>end: return

st(right)->create(right)->para(right)->cof(right)->sence(right)->e

```
<br>

----

**DON'T CHANGE THE ORDER OF FUNCTIONS IN THIS SUBROUTINE**
<br>
```flow
st=>start: init_constraint()
eq=>subroutine: addEqConstraint()
x=>subroutine: addXConstraint()
y=>subroutine: addYConstraint()
e=>end: return
st(right)->eq(right)->x(right)->y(right)->e
```
<br>

----

<br>
```flow
st=>start: optimize_all()
Xr=>operation: update Xr
cof=>operation: change coefficients
based on their position
rhs=>operation: change rhs
opti=>operation: Optimize model and get results
cond=>condition: is all cases 
optimized?
e=>end: return

st(right)->Xr(right)->cof(right)->rhs(right)->opti(right)->cond->
cond(yes)->e
cond(no,right)->Xr

```
<br>

----

<br>
```flow
st=>start: optimize_partOf()
Xr=>operation: update Xr
cof=>operation: change coefficients
based on their position
rhs=>operation: change rhs
opti=>operation: Optimize model and get results
cond=>condition: is all cases 
in the list
optimized?
e=>end: return

st(right)->Xr(right)->cof(right)->rhs(right)->opti(right)->cond->
cond(yes)->e
cond(no,right)->Xr

```