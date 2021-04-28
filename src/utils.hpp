
namespace genrack {
namespace util {


inline int int_div_round_up(int x, int y) {
	return x / y + (x % y != 0);
}

} // namespace util
} // namespace genrack
