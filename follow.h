// EVNELOPE FOLLOWER //////////////////////////////////////////////////////////////////////
// Derived from https://kferg.dev/posts/2020/audio-reactive-programming-envelope-followers/

class Follower {
public:
  Follower(float attack_kof, float decay_kof) :
    _attack_kof(attack_kof), 
    _decay_kof(decay_kof), 
    _out(0) 
    {}

  float Process(float in) {
    const auto abs_in = abs(in);
    if (in > _out) _out = _attack_kof * _out + (1.f - _attack_kof) * abs_in;
    else _out = _decay_kof * _out + (1.f - _decay_kof) * abs_in;
    return _out;
  }

private:
  float _attack_kof; 
  float _decay_kof;
  float _out;
};
