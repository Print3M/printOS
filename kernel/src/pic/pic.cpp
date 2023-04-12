
namespace pic {

extern "C" void __disable_pic(void);

void disable() {
	__disable_pic();
}

} // namespace pic