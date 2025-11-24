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

    ExpressionResult EvaluateExpression(const char* expression)
    {
        ExpressionResult result;
        result.valid = false;
        result.value = 0.0;
        result.errorMessage = "";

        if (!expression || expression[0] == '\0')
        {
            result.errorMessage = "Empty expression";
            return result;
        }

        // Parse the expression to extract left operand, operator, and right operand
        double leftOperand = 0.0;
        double rightOperand = 0.0;
        char op = '\0';
        bool foundOperator = false;
        bool isNegative = false;
        int32_t i = 0;

        // Skip leading whitespace
        while (expression[i] == ' ')
            ++i;

        // Check if first character is negative sign
        if (expression[i] == '-')
        {
            isNegative = true;
            ++i;
        }

        // Parse left operand
        bool hasDigit = false;
        bool hasDecimal = false;
        double decimalPlace = 0.1;

        while (expression[i] != '\0')
        {
            char c = expression[i];

            if (c >= '0' && c <= '9')
            {
                hasDigit = true;
                if (hasDecimal)
                {
                    leftOperand += (c - '0') * decimalPlace;
                    decimalPlace *= 0.1;
                }
                else
                {
                    leftOperand = leftOperand * 10.0 + (c - '0');
                }
            }
            else if (c == '.' && !hasDecimal)
            {
                hasDecimal = true;
            }
            else if (c == ' ')
            {
                // Skip whitespace
            }
            else if (c == '+' || c == '-' || c == '*' || c == '/')
            {
                op = c;
                foundOperator = true;
                ++i;
                break;
            }
            else
            {
                result.errorMessage = "Invalid character";
                return result;
            }
            ++i;
        }

        if (!hasDigit)
        {
            result.errorMessage = "Invalid number";
            return result;
        }

        if (isNegative)
            leftOperand = -leftOperand;

        if (!foundOperator)
        {
            result.errorMessage = "No operator found";
            return result;
        }

        // Skip whitespace after operator
        while (expression[i] == ' ')
            ++i;

        // Check for negative right operand
        bool rightIsNegative = false;
        if (expression[i] == '-')
        {
            rightIsNegative = true;
            ++i;
        }

        // Parse right operand
        hasDigit = false;
        hasDecimal = false;
        decimalPlace = 0.1;

        while (expression[i] != '\0')
        {
            char c = expression[i];

            if (c >= '0' && c <= '9')
            {
                hasDigit = true;
                if (hasDecimal)
                {
                    rightOperand += (c - '0') * decimalPlace;
                    decimalPlace *= 0.1;
                }
                else
                {
                    rightOperand = rightOperand * 10.0 + (c - '0');
                }
            }
            else if (c == '.' && !hasDecimal)
            {
                hasDecimal = true;
            }
            else if (c == ' ')
            {
                // Skip whitespace
            }
            else
            {
                result.errorMessage = "Invalid character in right operand";
                return result;
            }
            ++i;
        }

        if (!hasDigit)
        {
            result.errorMessage = "Invalid right operand";
            return result;
        }

        if (rightIsNegative)
            rightOperand = -rightOperand;

        // Perform the operation
        switch (op)
        {
            case '+':
                result.value = Add(leftOperand, rightOperand);
                result.valid = true;
                break;

            case '-':
                result.value = Subtract(leftOperand, rightOperand);
                result.valid = true;
                break;

            case '*':
                result.value = Multiply(leftOperand, rightOperand);
                result.valid = true;
                break;

            case '/':
            {
                OperationResult divResult = Divide(leftOperand, rightOperand);
                if (divResult.valid)
                {
                    result.value = divResult.value;
                    result.valid = true;
                }
                else
                {
                    result.errorMessage = "Division by zero";
                }
                break;
            }

            default:
                result.errorMessage = "Unknown operator";
                break;
        }

        return result;
    }
}

