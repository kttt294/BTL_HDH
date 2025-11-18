#include "calculator.h"

namespace
{
    constexpr double EPSILON = 1e-9;

    double Absolute(double value)
    {
        return value < 0.0 ? -value : value;
    }

    double SquareRoot(double value)
    {
        if (value <= 0.0)
            return 0.0;

        double guess = value;
        for (int i = 0; i < 16; ++i)
            guess = 0.5 * (guess + value / guess);

        return guess;
    }
}

namespace Calculator
{
    double Add(double lhs, double rhs)
    {
        return lhs + rhs;
    }

    double Subtract(double lhs, double rhs)
    {
        return lhs - rhs;
    }

    double Multiply(double lhs, double rhs)
    {
        return lhs * rhs;
    }

    OperationResult Divide(double lhs, double rhs)
    {
        OperationResult result;
        if (Absolute(rhs) < EPSILON)
        {
            result.valid = false;
            result.value = 0.0;
            return result;
        }

        result.valid = true;
        result.value = lhs / rhs;
        return result;
    }

    LinearSolution SolveLinear(double a, double b)
    {
        LinearSolution solution;
        solution.root = 0.0;

        if (Absolute(a) < EPSILON)
        {
            if (Absolute(b) < EPSILON)
            {
                solution.hasSolution = true;
                solution.infiniteSolutions = true;
            }
            else
            {
                solution.hasSolution = false;
                solution.infiniteSolutions = false;
            }
            return solution;
        }

        solution.hasSolution = true;
        solution.infiniteSolutions = false;
        solution.root = -b / a;
        return solution;
    }

    QuadraticSolution SolveQuadratic(double a, double b, double c)
    {
        QuadraticSolution solution;
        solution.discriminant = 0.0;
        solution.root1 = 0.0;
        solution.root2 = 0.0;
        solution.reducedToLinear = false;

        if (Absolute(a) < EPSILON)
        {
            solution.reducedToLinear = true;
            LinearSolution linear = SolveLinear(b, c);
            solution.hasRealSolutions = linear.hasSolution;
            if (linear.hasSolution)
            {
                solution.root1 = linear.root;
                solution.root2 = linear.root;
            }
            return solution;
        }

        double discriminant = b * b - 4.0 * a * c;
        solution.discriminant = discriminant;

        if (discriminant < -EPSILON)
        {
            solution.hasRealSolutions = false;
            return solution;
        }

        double sqrtDisc = SquareRoot(discriminant < 0.0 ? 0.0 : discriminant);
        double denom = 2.0 * a;

        solution.hasRealSolutions = true;
        solution.root1 = (-b + sqrtDisc) / denom;
        solution.root2 = (-b - sqrtDisc) / denom;
        return solution;
    }
}
