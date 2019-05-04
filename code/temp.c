switch (resolveFunc(p->data.function.name)) {
                case NEG_OPER:
                    p->data.function.op1 =(-1) * p->data.function.op1
                    return p;
                case ABS_OPER:
                    return fabs(eval(p->data.function.op1));
                case EXP_OPER:
                    return exp(eval(p->data.function.op1));
                case SQRT_OPER:
                    return sqrt(eval(p->data.function.op1));
                case ADD_OPER:
                    return ((eval(p->data.function.op1)) + (eval(p->data.function.op2)));
                case SUB_OPER:
                    return ((eval(p->data.function.op1)) - (eval(p->data.function.op2)));
                case MULT_OPER:
                    return ((eval(p->data.function.op1)) * (eval(p->data.function.op2)));
                case DIV_OPER:
                    return ((eval(p->data.function.op1)) / (eval(p->data.function.op2)));
                case REMAINDER_OPER:
                    return remainder((eval(p->data.function.op1)), (eval(p->data.function.op2)));
                case LOG_OPER:
                    return (log(eval(p->data.function.op1)));
                case POW_OPER:
                    return (pow((eval(p->data.function.op1)), (eval(p->data.function.op2))));
                case MAX_OPER:
                    if ((eval(p->data.function.op1)) > (eval(p->data.function.op2))) {
                        return eval(p->data.function.op1);
                    } else {
                        return eval(p->data.function.op2);
                    }
                case MIN_OPER:
                    if ((eval(p->data.function.op1)) < (eval(p->data.function.op2))) {
                        return eval(p->data.function.op1);
                    } else {
                        return eval(p->data.function.op2);
                    }
                case EXP2_OPER:
                    return exp2((eval(p->data.function.op1)));
                case CBRT_OPER:
                    return (cbrt(eval(p->data.function.op1)));
                case HYPOT_OPER:
                    return (hypot((eval(p->data.function.op1)), (eval(p->data.function.op2)))); 
 }
//below this line was the original
switch (p->type) {
        case NUM_TYPE:
            return p->data.number.value;
        case FUNC_TYPE:
            switch (resolveFunc(p->data.function.name)) {
                case NEG_OPER:
                    return ((-1) * eval(p->data.function.op1));
                case ABS_OPER:
                    return fabs(eval(p->data.function.op1));
                case EXP_OPER:
                    return exp(eval(p->data.function.op1));
                case SQRT_OPER:
                    return sqrt(eval(p->data.function.op1));
                case ADD_OPER:
                    return ((eval(p->data.function.op1)) + (eval(p->data.function.op2)));
                case SUB_OPER:
                    return ((eval(p->data.function.op1)) - (eval(p->data.function.op2)));
                case MULT_OPER:
                    return ((eval(p->data.function.op1)) * (eval(p->data.function.op2)));
                case DIV_OPER:
                    return ((eval(p->data.function.op1)) / (eval(p->data.function.op2)));
                case REMAINDER_OPER:
                    return remainder((eval(p->data.function.op1)), (eval(p->data.function.op2)));
                case LOG_OPER:
                    return (log(eval(p->data.function.op1)));
                case POW_OPER:
                    return (pow((eval(p->data.function.op1)), (eval(p->data.function.op2))));
                case MAX_OPER:
                    if ((eval(p->data.function.op1)) > (eval(p->data.function.op2))) {
                        return eval(p->data.function.op1);
                    } else {
                        return eval(p->data.function.op2);
                    }
                case MIN_OPER:
                    if ((eval(p->data.function.op1)) < (eval(p->data.function.op2))) {
                        return eval(p->data.function.op1);
                    } else {
                        return eval(p->data.function.op2);
                    }
                case EXP2_OPER:
                    return exp2((eval(p->data.function.op1)));
                case CBRT_OPER:
                    return (cbrt(eval(p->data.function.op1)));
                case HYPOT_OPER:
                    return (hypot((eval(p->data.function.op1)), (eval(p->data.function.op2))));
            }
        case SYMB_TYPE:

            temp = resolveSymbol(p->data.symbol.name, p);
            if (temp == NULL) {
                puts("there is an error");
                exit(1);
            }
            return eval(temp->val);
    }
    return 0.0;
