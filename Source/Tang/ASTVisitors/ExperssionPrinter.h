#pragma once

#include <iostream>
#include <sstream>
#include "../ASTs/CoreASTs.h"

XC_BEGIN_NAMESPACE_1(Tang)
{
    class ProgramPrinter : public IVisitor
    {
    public:
        void Visit(Program* node) override
        {
            for (auto itr : node->mStatements)
            {
                itr->Accept(this);
            }
        }

        void Visit(ExpressionStatement* node) override
        {
            node->mExpression->Accept(this);
            std::cout << ";\n";
        }

        void Visit(NumberExpression* node) override
        {
            std::cout << node->mValue;
        }

        void Visit(VariableExpression* node) override
        {
            std::cout << node->mName;
        }

        void Visit(CalculateExpression* node) override
        {
            std::cout << "(";
            node->mLeftExpression->Accept(this);
            std::string s;
            switch (node->mOperator)
            {
            case CalculateExpression::Operator::Plus:
                s = "+";
                break;
            case CalculateExpression::Operator::Minus:
                s = "-";
                break;
            case CalculateExpression::Operator::Multiply:
                s = "*";
                break;
            case CalculateExpression::Operator::Divide:
                s = "/";
                break;
            default:
                break;
            }

            std::cout << " " << s << " ";
            node->mRightExpression->Accept(this);
            std::cout << ")";
        }

        void Visit(AssignExpression* node) override
        {
            std::cout << "(";
            node->mLeftExpression->Accept(this);

            std::cout << "=";

            node->mRightExpression->Accept(this);
            std::cout << ")";
        }
    };

    class ExpressionAnswer : public IVisitor
    {
    public:
        void Visit(Program* node) override
        {
            for (auto itr : node->mStatements)
            {
                itr->Accept(this);
            }
        }

        void Visit(ExpressionStatement* node) override
        {
            node->mExpression->Accept(this);
        }

        void Visit(BlockStatement* node) override
        {
            for (auto itr : node->mStatements)
            {
                itr->Accept(this);
            }
        }

        void Visit(WhileStatement* node) override
        {
            while (true)
            {
                node->mConditionExpression->Accept(this);
                if (mLastResult == 0.0)
                {
                    break;
                }
                else
                {
                    node->mBodyStatement->Accept(this);
                }
            }
        }

        void Visit(NumberExpression* node) override
        {
            mLastResult = node->mValue;
        }

        void Visit(VariableExpression* node) override
        {
            if (!mVariablesValues.count(node->mName))
            {
                mVariablesValues[node->mName] = 0.0;
            }
            else
            {
                mLastResult = mVariablesValues[node->mName];
            }

            mLastVariableName = node->mName;
        }

        void Visit(CalculateExpression* node) override
        {
            node->mLeftExpression->Accept(this);
            double leftResult = mLastResult;
            node->mRightExpression->Accept(this);
            double rightResult = mLastResult;
            switch (node->mOperator)
            {
            case CalculateExpression::Operator::Plus:
                mLastResult = leftResult + rightResult;
                break;
            case CalculateExpression::Operator::Minus:
                mLastResult = leftResult - rightResult;
                break;
            case CalculateExpression::Operator::Multiply:
                mLastResult = leftResult * rightResult;
                break;
            case CalculateExpression::Operator::Divide:
                mLastResult = leftResult / rightResult;
                break;
            default:
                break;
            }
        }

        void Visit(CompareExpression* node) override
        {
            node->mLeftExpression->Accept(this);
            double leftResult = mLastResult;
            node->mRightExpression->Accept(this);
            double rightResult = mLastResult;
            switch (node->mOperator)
            {
            case  CompareExpression::Operator::Greater:
                mLastResult = leftResult > rightResult ? 1 : 0;
                break;
            case CompareExpression::Operator::Lesser:
                mLastResult = leftResult < rightResult ? 1 : 0;
                break;
            default:
                break;
            }
        }

        void Visit(AssignExpression* node) override
        {
            node->mLeftExpression->Accept(this);
            std::string str = mLastVariableName;
            node->mRightExpression->Accept(this);
            std::cout << str << std::endl;
            mVariablesValues[str] = mLastResult;
        }

    public:
        auto GetVarialbeValues()
        {
            return mVariablesValues;
        }

        auto GetResult()
        {
            std::string ans;
            std::stringstream s(ans);
            for (auto itr : mVariablesValues)
            {
                s<< itr.first << std::string(": ") <<(itr.second) << "\n";
            }

            return s.str();
        }

    private:
        std::map<String, double> mVariablesValues;
        String mLastVariableName;
        double mLastResult;
    };
} XC_END_NAMESPACE_1;