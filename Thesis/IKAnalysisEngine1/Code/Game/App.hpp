#pragma once
//


class App
{
public:
	App();
	~App();
	bool m_isQuitting;
	bool m_hasFocus;
	void setIsQuitting(bool isNowQuitting);
	void RunFrame();
	void OnKeyPressed(unsigned char keyCode);
	void OnKeyReleased(unsigned char keyCode);
	void OnGainedFocus();
	void OnLostFocus();
private:
	void BeginFrame();
	void EndFrame();
	void Update(float deltaSeconds);
	void Render() const;

private:
	
};