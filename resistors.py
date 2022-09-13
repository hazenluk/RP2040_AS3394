import copy
from itertools import permutations
import itertools

def vout(v1, v2, v3, r1, r2, r3, r4):
    return (v1*r2*r3*r4+v2*r1*r3*r4-v3*r1*r2*r4) / (r2*r3*r4+r1*r3*r4+r1*r2*r4+r1*r2*r3)

E24 = [1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0, 2.2, 2.4, 2.7, 3.0, 3.3, 3.6, 3.9, 4.3, 4.7, 5.1, 5.6, 6.2, 6.8, 7.5, 8.2, 9.1]
min_error = 1000000000
best_params = []
rvalues = copy.deepcopy(E24)
for x in range(1, 6):
    for val in E24:
        rvalues.append(val*pow(10, x))

# print(vout(3.3, 3.3, 1, 5, 10, 15, 20)) # should return 2.216
count = 0
for perm in itertools.permutations(rvalues, 4):
    for vneg in [1, 3.3, 5]:
        va = vout(0, 0, vneg, perm[0], perm[1], perm[2], perm[3])
        vb = vout(0, 3.3, vneg, perm[0], perm[1], perm[2], perm[3])
        vc = vout(3.3, 0, vneg, perm[0], perm[1], perm[2], perm[3])
        vd = vout(3.3, 3.3, vneg, perm[0], perm[1], perm[2], perm[3])
        '''MSE = ((-1-va)**2 + (0.5-vb)**2 + (2-vc)**2 + (3.3-vd)**2)/4
        if MSE < min_error:
            min_error = MSE
            best_params = [va, vb, vc, vd, vneg] + list(perm)
        '''
        if va < -.5 and vb > - 0.1 and vb < 0.95 and vc > 1.55 and vc < 2.2 and vd > 3.2:
            well shit this won't work lmao, no way I'm getting vd > 3.2
    count += 1
    if(count%412293 == 0):
        print(str((count/412293024)*100) + '% Complete')
print(best_params)
print("MSE" + str(min_error))