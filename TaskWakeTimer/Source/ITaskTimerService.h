

class ITaskTimerService
{
public:
	virtual void Sleep(uint32_t a_ThreadIndex, uint32_t a_TimeSec) = 0;
};
