#ifndef _CALCULATOR_H_
#define _CALCULATOR_H_

#include "types.h"

struct OperationResult
{
    bool valid;
    double value;
};

struct LinearSolution
{
    bool hasSolution;
    bool infiniteSolutions;
    double root;
};

struct QuadraticSolution
{
    bool hasRealSolutions;
    bool reducedToLinear;
    double discriminant;
    double root1;
    double root2;
};

struct ExpressionResult
{
    bool valid;
    double value;
    const char* errorMessage;
};

namespace Calculator
{
    double Add(double lhs, double rhs);
    double Subtract(double lhs, double rhs);
    double Multiply(double lhs, double rhs);
    OperationResult Divide(double lhs, double rhs);

    LinearSolution SolveLinear(double a, double b);
    QuadraticSolution SolveQuadratic(double a, double b, double c);

    ExpressionResult EvaluateExpression(const char* expression);
}

#endif
