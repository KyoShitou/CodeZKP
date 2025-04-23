def xgcd( x, y ):
    old_r, r = (x, y)
    old_s, s = (1, 0)
    old_t, t = (0, 1)
    while r != 0:
        quotient = old_r // r
        old_r, r = (r, old_r - quotient * r)
        old_s, s = (s, old_s - quotient * s)
        old_t, t = (t, old_t - quotient * t)

        print(f"{old_r}, {old_s}, {old_t}")
        print(f"{r}, {s}, {t}")

    return old_s, old_t, old_r # a, b, g


class FieldElement:
    def __init__( self, value, field ):
        self.value = value % field.p
        self.field = field

    def __add__( self, right ):
        return self.field.add(self, right)

    def __mul__( self, right ):
        return self.field.multiply(self, right)

    def __sub__( self, right ):
        return self.field.subtract(self, right)

    def __truediv__( self, right ):
        return self.field.divide(self, right)

    def __neg__( self ):
        return self.field.negate(self)

    def inverse( self ):
        return self.field.inverse(self)

    # modular exponentiation -- be sure to encapsulate in parentheses!
    def __xor__( self, exponent ):
        if type(exponent) == FieldElement:
            exponent = exponent.value
        acc = FieldElement(1, self.field)
        val = FieldElement(self.value, self.field)
        # for i in reversed(range(len(bin(exponent)[2:]))):
        #     acc = acc * acc
        #     if (1 << i) & exponent != 0:
        #         acc = acc * val

        #     print(f"{acc}, {val}")

        while (exponent > 0):
            if (exponent % 2 == 1):
                acc = acc * val
            val = val * val
            exponent = exponent // 2
            print(f"{acc}, {val}")
        return acc

    def __eq__( self, other ):
        return self.value == other.value

    def __neq__( self, other ):
        return self.value != other.value

    def __str__( self ):
        return str(self.value)

    def __bytes__( self ):
        return bytes(str(self).encode())

    def is_zero( self ):
        if self.value == 0:
            return True
        else:
            return False

class Field:
    def __init__( self, p ):
        self.p = p

    def zero( self ):
        return FieldElement(0, self)

    def one( self ):
        return FieldElement(1, self)

    def multiply( self, left, right ):
        return FieldElement((left.value * right.value) % self.p, self)

    def add( self, left, right ):
        return FieldElement((left.value + right.value) % self.p, self)

    def subtract( self, left, right ):
        return FieldElement((self.p + left.value - right.value) % self.p, self)

    def negate( self, operand ):
        return FieldElement((self.p - operand.value) % self.p, self)

    def inverse( self, operand ):
        a, b, g = xgcd(operand.value, self.p)
        return FieldElement(a, self)

    def divide( self, left, right ):
        assert(not right.is_zero()), "divide by zero"
        a, b, g = xgcd(right.value, self.p)
        return FieldElement(left.value * a % self.p, self)
    

    def generator( self ):
        assert(self.p == 1 + 407 * ( 1 << 119 )), "Do not know generator for other fields beyond 1+407*2^119"
        return FieldElement(85408008396924667383611388730472331217, self)
        
    def primitive_nth_root( self, n ):
        if self.p == 1 + 407 * ( 1 << 119 ):
            assert(n <= 1 << 119 and (n & (n-1)) == 0), "Field does not have nth root of unity where n > 2^119 or not power of two."
            root = FieldElement(85408008396924667383611388730472331217, self)
            order = 1 << 119
            while order != n:
                root = root^2
                order = order/2
            return root
        else:
            assert(False), "Unknown field, can't return root of unity."

    def generator( self ):
        assert(self.p == 1 + 407 * ( 1 << 119 )), "Do not know generator for other fields beyond 1+407*2^119"
        return FieldElement(85408008396924667383611388730472331217, self)
        
    def primitive_nth_root( self, n ):
        if self.p == 1 + 407 * ( 1 << 119 ):
            assert(n <= 1 << 119 and (n & (n-1)) == 0), "Field does not have nth root of unity where n > 2^119 or not power of two."
            root = FieldElement(85408008396924667383611388730472331217, self)
            order = 1 << 119
            while order != n:
                root = root^2
                order = order/2
            return root
        else:
            assert(False), "Unknown field, can't return root of unity."


def test_inv():
    p = 1 + 407 * ( 1 << 119 ) # 1 + 11 * 37 * 2^119
    f = Field(p)

    print("Test Inverse")
    print(FieldElement(1, f).inverse())
    print(FieldElement(p-1, f).inverse())

    val1 = FieldElement(123456789012345678901234567890123456789, f)
    val2 = FieldElement(987654321098765432109876543210987654321, f)

    print(val1 * val1.inverse())

    print(val2 * val2.inverse())

    print(val1.inverse().inverse())

def test_exp():

    p = 1 + 407 * ( 1 << 119 ) 
    f = Field(p)

    two = FieldElement(2, f)
    three = FieldElement(3, f)
    val1 = FieldElement(123456789012345678901234567890123456789, f)
    neg_1 = FieldElement(p-1, f)

    print(two^(p - 1))

def test_primitive_root():
    order_pow2 = 1 << 119
    p = 1 + 407 * ( 1 << 119 ) 
    f = Field(p)
    root_pow2 = f.primitive_nth_root(order_pow2)
    print(root_pow2^FieldElement(order_pow2, f))

    order_half_pow2 = order_pow2 // 2
    print(root_pow2^FieldElement(order_half_pow2, f))


test_primitive_root()