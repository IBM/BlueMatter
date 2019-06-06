from __future__ import print_function
# function to carry out polynomial fits using singular value decomposition

from Numeric import *
from LinearAlgebra import *
import exceptions
import types

class PolyFitException(exceptions.Exception):
    def __init__(self, args=None):
        self.args = args

# polynomial evalation via Horner's method from a coefficient array
def polyeval(coeff, x):
    sum = 0
    for i in range(1,len(coeff)):
        sum = (sum + coeff[len(coeff)-i])*float(x)
    sum = sum + coeff[0]
    return sum

# function to carry out polynomial fits using singular value decomposition
# in addition to setting the degree of the polynomial,
# the user can also provide a mask to "turn off" certain terms in the polynomial
# only those terms whose corresponding offsets into the mask have a "1" will be used
# By default all terms up to polydeg are used
# returns a tuple containing (solution, covarianceMatrix)
def polyfit(xdata, ydata, sigma, polydeg, mask = 0, rcond=1e-10):
    print("polyfit xdata:", xdata)
    print("polyfit ydata:", ydata)
    sigmaProvided = 1
    if len(xdata) != len(ydata):
        raise PolyFitException("inconsistent sizes of data arrays")
    # construct the mask, if necessary
    if mask == 0:
        mask = ones(polydeg+1, Int)
    else:
        if len(mask) != polydeg+1:
            raise PolyFitException("inconsistent mask size")
    # construct the design matrix A
    a = []

    if isinstance(sigma, types.FloatType) or isinstance(sigma, types.IntType):
        if float(sigma) < 0:
            sigmaProvided = 0
            sigma = 1.0
        invsigma = zeros(len(xdata),Float) + 1.0/float(sigma)
    else:
        if len(sigma) != len(xdata):
            raise PolyFitException("inconsistent size of error array")
        invsigma = array(sigma,Float)
        invsigma = 1.0/invsigma

    # print "invsigma:", invsigma
    
    for i in range(len(xdata)):
        a.append([])
        powArray = []
        for j in range(polydeg+1):
            if j == 0:
                powArray.append(invsigma[i])
                if mask[j]:
                    a[i].append(invsigma[i])
            else:
                powArray.append(powArray[j-1]*float(xdata[i]))
                if mask[j]:
                    a[i].append(powArray[j])

    # print "a: ", a
    model = array(a, Float)
    b = array(ydata, Float)
    b = b*invsigma

    # debugging: check answer using routine from LinearAlgebra module
    # answer = linear_least_squares(model, b)
    # lsq = answer[0]
    # var = answer[1]
    # rankA = answer[2]
    # singular = answer[3]
    # print "least squares solution =",lsq
    # print "sum of squared residuals =",var
    # print "Rank of design matrix =",rankA
    # print "Singular values of design matrix =",singular

    svd = singular_value_decomposition(model)

    # print "svd: ", svd
    u = svd[0]
    wdiag = svd[1]
    vtrans = svd[2]
    
    wabs = abs(wdiag)
    wmax = max(wabs)
                    
    utrans = transpose(u)
    m = int(shape(u)[0])
    n = int(shape(u)[1])

    weight = zeros(n, Float)
    for i in range(n):
        if  (abs(wdiag[i]/wmax) > rcond):
            weight[i] = 1.0/wdiag[i]

    solution = []
    for i in range(n):
        param = 0
        for j in range(n):
            param = param + innerproduct(utrans[j],b)*weight[j]*vtrans[j][i]
        solution.append(param)
    print("solution =", solution)

    cov = zeros([n,n], Float)
    for i in range(n):
        for j in range(n):
            for k in range(n):
                cov[i][j] = cov[i][j]+vtrans[k][i]*vtrans[k][j]*weight[k]*weight[k]

#    print "covariance = ",cov
    return (solution, cov)

