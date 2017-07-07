
class SceneBase {
public:
	enum StateResult {
		StateResult_Finish,     // ‘JˆÚI—¹
		StateResult_Continue    // ‘JˆÚ’†
	};

	virtual StateResult update() = 0;   // XV
};
