//����������
#pragma once

#include <assert.h>

/*************************************************************************
****                      ����Ϊ���ֲ��η�����                     
****                   ѭ�����͵�û��ended()�ӿ�                   
****������Ҫע�Ⲩ�α߽�ֵ��dTime����֮���ֵ�Ƿ񳬳���һ�ٽ�ʱ��
****������ʱ��߽��ж���ѭ��ǰ�պ󿪡���ԭ��                       
*************************************************************************/

//ʱ�������巢��������Ҫ���ڼ������ʱ������ʲô��
//������ͼ��		|_|_|_|_|_|_|_|_|_|_|_

class CTimePulseGenerator
{
	void	setArg(float duration, float initPhrase = 0)	{
		_duration = duration;
		_timer = initPhrase;
		_run = true;
		_last_val = false;
	}
	bool	update(float dTime){
		_last_val = false;
		if (_run)		{
			_timer -= dTime;
			if (_timer <= 0.0)
				_timer += _duration;
			_last_val = true;
		}
		return _last_val;
	}

	void	reset()	{_timer = _duration;}
	void	countdown()	{ _timer = 0;	}

	void	enable(bool b)	{ _run = b;	}
	bool	enabled()	{ return _run;	}

	bool	lastUpdate(){ return _last_val;	}

private:
	float	_duration;
	float	_timer;
	bool	_run;
	bool	_last_val;
};


#if 0
//*************************************************************

//֡������巢��������Ҫ���ڼ������֡����ʲô��
//������ͼ��		|_|_|_|_|_|_|_|_|_|_|_
class	CFramePulseGenerator:
	def		__init__(self, duration, initPhrase = 0):
		assert 0 <= initPhrase <= duration
		self._duration = duration
		self._timer =  initPhrase
		self._run = True
		self._last_val = False
	
	def		reset(self):
		self._timer = self._duration
	
	def		countdown(self):
		self._timer = 0
	
	def		enable(self, b):
		self._run = b
	
	def		enabled(self):
		return self._run
	
	def		update(self):
		self._last_val = False
		if self._run:
			self._timer -= 1
			if self._timer <= 0:
				self._timer = self._duration
				self._last_val = True
				return True
		return False
	
	def		lastUpdate(self):
		return self._last_val
	
	def		timer(self):
		return self._timer

//*************************************************************

//ʱ�䵹��ʱ����ֻ�����һ������
//������ͼ��		_______|--------
class	CTimeCountDownGenerator:
	def		__init__(self, duration):
		assert duration > 0, duration
		self._duration = duration
		self._timer = duration
		self._last_val = False
		self._run = True
	
	def		reset(self, new_duration = None):
		if new_duration is not None:
			self._duration = new_duration
		
		self._timer = self._duration
		self._last_val = False
	
	def		countdown(self):
		self._timer = 0
		self._last_val = True
	
	def		enable(self, b):
		self._run = b
	
	def		enabled(self):
		return self._run
	
	def		ended(self):
		return self._timer <= 0
	
	def		update(self, dTime):
		if self._run:
			if self._timer > 0:
				self._timer -= dTime
				if self._timer <= 0:
					self._last_val = True
		return self._last_val
	
	def		lastUpdate(self):
		return self._last_val


//*************************************************************

//֡����ʱ����ֻ�����һ������
//������ͼ��		_______|--------
class	CFrameCountDownGenerator:
	def		__init__(self, duration):
		assert duration > 0, duration
		self._duration = duration
		self._timer = duration
		self._last_val = False
		self._run = True
	
	def		reset(self, new_duration = None):
		if new_duration is not None:
			self._duration = new_duration
		
		self._timer = self._duration
		self._last_val = False
	
	def		countdown(self):
		self._timer = 0
		self._last_val = True
	
	def		enable(self, b):
		self._run = b
	
	def		enabled(self):
		return self._run
	
	def		ended(self):
		return self._timer <= 0
	
	def		update(self, dTime):
		if self._run:
			if self._timer > 0:
				self._timer -= 1
				if self._timer <= 0:
					self._last_val = True
		return self._last_val
	
	def		lastUpdate(self):
		return self._last_val

//*************************************************************

//����������
//������ͼ��		____|------|_____|------|_____	amplitude > 0
//������ͼ��		----|______|-----|______|-----	amplitude < 0
class	CSquareWaveGenerator:
	def		__init__(self, upEdge, downEdge, amplitude = 1.0, YOffset = 0.0, loop = 1):
		assert 0 <= upEdge < downEdge
		
		#self._t0	= 0
		self._t1	= upEdge
		self._t2	= downEdge
		self._A		= amplitude
		self._Y0	= YOffset
		
		self._loopCount = 0
		self._loops  = loop
		self._timer = 0
		self._run = True
		self._last_val = self._Y0
	
	def		reset(self):
		self._loopCount = 0
		self._timer = 0
		self._last_val = self._Y0
		return True, self._Y0
	
	def		terminate(self):
		self._timer = self._t2
		self._last_val = self._Y0
		return False, self._last_val
	
	def		enable(self, b):
		self._run = b
	
	def		enabled(self):
		return self._run
	
	def		update(self, dTime):	#dTime ���ܳ���һ�����ڵĳ��ȣ���֮ duration ���ܵ��� dTime = 1.0/fps ����С 16.67ms
		if self._run:
			if self._timer < self._t2:				#��Чʱ���ڣ��ż�ʱ
				self._timer += dTime
				
				if self._timer < self._t1:			#������ǰ��
					return True, self._Y0
				
				if self._timer < self._t2:			#�½���ǰ��
					self._last_val = self._Y0 + self._A
					return True, self._last_val
				
				#����ѭ��
				if self._loops <= 0:						#�½��غ���/�����ڵ�������ǰ��
					self._timer -= self._t2
					assert self._timer < self._t2
					return self.update(0)
				
				#������ѭ��
				self._loopCount += 1
				if self._loopCount < self._loops:			#�½��غ���/�����ڵ�������ǰ��
					self._timer -= self._t2
					assert self._timer < self._t2
					return self.update(0)
			
			return False, self._Y0					#end
		
		return self._timer < self._t2, self._last_val
	
	def		lastUpdate(self):
		return self._last_val

//*************************************************************

//���β�����������Ҫ���ڵ��뵭�����ƣ�����alhpa����ʱҪע��amplitude��YOffset��ȡֵ��Χ��������update����ֵ�ĵڶ���ֵ����[0, 1]��Χ
//������ͼ��		____/------\_____/------\_____	amplitude > 0
//������ͼ��		----\______/-----\______/-----	amplitude < 0
//��������ģ�����ǲ�
class	CTrapeziformWaveGenerator:
	def		__init__(self, raisingEdgeBegin, raisingEdgeEnd, fallingEdgeBegin, fallingEdgeEnd, amplitude = 1.0, YOffset = 0.0, loop = 1):
		assert 0 <= raisingEdgeBegin <= raisingEdgeEnd <= fallingEdgeBegin <= fallingEdgeEnd
		
		#self._t0	= 0
		self._t1	= raisingEdgeBegin
		self._t2	= raisingEdgeEnd
		self._t3	= fallingEdgeBegin
		self._t4	= fallingEdgeEnd
		self._A		= amplitude
		self._Y0	= YOffset
		
		self._loopCount = 0
		self._loops  = loop
		self._timer = 0
		self._run = True
		self._last_val = self._Y0
	
	def		reset(self):
		self._loopCount = 0
		self._timer = 0
		self._last_val = self._Y0
		return True, self._Y0
	
	def		terminate(self):
		self._timer = self._t4
		self._last_val = self._Y0
		return False, self._last_val
	
	def		enable(self, b):
		self._run = b
	
	def		enabled(self):
		return self._run
	
	def		update(self, dTime):	#dTime ���ܳ���һ�����ڵĳ��ȣ���֮ duration ���ܵ��� dTime = 1.0/fps ����С 16.67ms
		if self._run:
			if self._timer < self._t4:				#��Чʱ���ڣ��ż�ʱ
				self._timer += dTime
				
				if self._timer < self._t1:			#������ǰ��
					return True, self._Y0
				
				if self._timer < self._t2:			#������
					y = (self._timer - self._t1) / (self._t2 - self._t1)
					self._last_val = self._Y0 + y * self._A
					return True, self._last_val
				
				if self._timer < self._t3:			#��ƽ���ֽ׶�
					self._last_val = self._Y0 + self._A
					return True, self._last_val
				
				if self._timer < self._t4:			#�½���
					y = (self._t4 - self._timer) / (self._t4 - self._t3)
					self._last_val = self._Y0 + y * self._A
					return True, self._last_val
				
				#����ѭ��
				if self._loops <= 0:						#�½��غ���/�����ڵ�������ǰ��
					self._timer -= self._t4
					assert self._timer < self._t4
					return self.update(0)
				
				#������ѭ��
				self._loopCount += 1
				if self._loopCount < self._loops:			#�½��غ���/�����ڵ�������ǰ��
					self._timer -= self._t4
					assert self._timer < self._t4
					return self.update(0)
			
			return False, self._Y0					#end
		
		return self._timer < self._t4, self._last_val
	
	def		lastUpdate(self):
		return self._last_val
	
	def		time(self):
		return self._timer
#endif

//*************************************************************

//��Ծ������������Ҫ���ڵ��뵭������
//������ͼ��		___/----	amplitude > 0
//������ͼ��		----\___	amplitude < 0
class	CRampStepWaveGenerator
{
public:
	void	setArg(float rampBegin, float rampEnd, float waveEnd, float amplitude = 1.0, float YOffset = 0.0)
	{
		//assert(0 <= rampBegin < rampEnd <= waveEnd);

		_t1	= rampBegin;
		_t2	= rampEnd;
		_t3	= waveEnd;
		_A	= amplitude;
		_Y0	= YOffset;

		_timer = 0;
		_run = true;
		_last_val = _Y0;
	}

	bool	update(float dTime) {
		if (_run)
		{
			if (_timer < _t3)
			{				//��Чʱ���ڣ��ż�ʱ
				_timer += dTime;

				if (_timer < _t1)			//ǰ��ƽ���ֽ׶�
				{
					_last_val = _Y0;
					return true;
				}
				if (_timer < _t2)			//��Ծ�׶�
				{
					float y = (_timer - _t1) / (_t2 - _t1);
					_last_val = _Y0 + y * _A;
					return true;
				}
				_last_val = _Y0 + _A;		//���ƽ���ֽ׶�
				return true;
			}
			return false;		//end
		}
		return _timer < _t3;
	}

	void	reset() { _timer = 0; _last_val = _Y0; }
	void	terminate() { _timer = _t3; _last_val = _Y0 + _A; }
	bool	ended()	{ return _timer >= _t3;	}

	void	enable(bool b)	{ _run = b;	}
	bool	enabled()	{ return _run;	}
	float	lastUpdate(){ return _last_val;	}

private:
		float _t1;
		float _t2;
		float _t3;
		float _A;
		float _Y0;

		float _timer;
		bool _run;
		float _last_val;
};
	

#if 0
//*************************************************************

//�������Բ�ֵ������Ҫ���ڶ������ƣ������������޹���ͼ��
class	CPolyLineInterpolator:
	def	__init__(self, timeStamps, points, amplitude = 1.0, YOffset = 0.0):
		assert len(timeStamps) >= 2, timeStamps
		assert len(timeStamps) == len(points), timeStamps
		if __debug__:		#timeStamps must be sorted
			for i in xrange(len(timeStamps) - 1):
				assert timeStamps[i] < timeStamps[i+1], timeStamps[i+1]
		
		#consts
		self._timeStamps = timeStamps
		self._points = points
		self._A		= amplitude
		self._Y0	= YOffset
		#internal vars
		self.reset()
	
	def		reset(self):
		self._nextIdx = 1
		self._timer = self._timeStamps[self._nextIdx - 1]
		self._last_val = self._Y0 + self._points[self._nextIdx - 1] * self._A
		return True, self._last_val
	
	def		terminate(self):
		self._nextIdx = len(self._timeStamps)
		self._timer = self._timeStamps[self._nextIdx - 1]
		self._last_val = self._Y0 + self._points[self._nextIdx - 1] * self._A
		return False, self._last_val
	
	def		update(self, dTime):
		if self._nextIdx < len(self._timeStamps):
			t = self._timer
			t1 = self._timeStamps[self._nextIdx - 1]
			t2 = self._timeStamps[self._nextIdx]
			y1 = self._points[self._nextIdx - 1]
			y2 = self._points[self._nextIdx]
			
			y = y1 + (y2 - y1) * (t - t1) / (t2 - t1)
			
			self._timer += dTime
			if self._timer >= t2:
				self._nextIdx += 1
				if self._nextIdx >= len(self._timeStamps):
					y = self._points[-1]
			
			self._last_val = self._Y0 + y * self._A
			
			return True, self._last_val
		
		return False, self._last_val
	
	def		lastUpdate(self):
		return self._last_val
	
	def		time(self):
		return self._timer
#endif