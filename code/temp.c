
        case SYMB_TYPE:

            temp = resolveSymbol(p->data.symbol.name, p);
            if (temp == NULL) {
                puts("there is an error");
                exit(1);
            }
            return eval(temp->val);
    }
    return 0.0;
