#include <cstdint>
#include <chrono>
#include <ostream>

namespace dr4 {
	class Timer
	{
	public:

		typedef std::chrono::high_resolution_clock HighResolutionClock_t;
		typedef std::chrono::milliseconds milliseconds_t;

		Timer(bool run = true) { if (run) reset(); }

		void reset()
		{
			m_start = HighResolutionClock_t::now();
		}

		milliseconds_t elapsed()const
		{
			return std::chrono::duration_cast<milliseconds_t>(HighResolutionClock_t::now() - m_start);
		}

		double milliseconds() const
		{
			return ((double)elapsed().count());
		}
		double seconds() const {
			return milliseconds() * 0.001;
		}

		//template <typename T, typename Traits>
		//friend std::basic_ostream<T, Traits>& operator<<(std::basic_ostream<T, Traits>& out, const Timer& timer)
		//{
		//	return out << timer.Elapsed().count();
		//}

		HighResolutionClock_t::time_point m_start;
	};
}
