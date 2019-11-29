#pragma once

enum class eParticleBuffer {
	kNone = 1 << 0,
	kData = 1 << 1,
	kAlive = 1 << 2,
	kSpawned = 1 << 3,
	kDeath = 1 << 4,
	kSubIndex = 1 << 5,

	kMaskSubBuffer = kAlive | kSpawned | kDeath,
	kAllBuffers = kData | kAlive | kSpawned | kDeath | kSubIndex,
	kAllSubBuffers = kAlive | kSpawned | kDeath | kSubIndex,
};

inline eParticleBuffer operator|(eParticleBuffer const lhs, eParticleBuffer const rhs) { return static_cast<eParticleBuffer> (static_cast<int>(lhs) | static_cast<int>(rhs)); }
inline eParticleBuffer const &operator|=(eParticleBuffer &lhs, eParticleBuffer const &rhs) {
	lhs = static_cast<eParticleBuffer> (static_cast<int>(lhs) | static_cast<int>(rhs));
	return (lhs);
}
inline eParticleBuffer operator&(eParticleBuffer const lhs, eParticleBuffer const rhs) { return static_cast<eParticleBuffer> (static_cast<int>(lhs) &  static_cast<int>(rhs)); }
inline eParticleBuffer operator&(int const lhs, eParticleBuffer const rhs) { return static_cast<eParticleBuffer> (lhs &  static_cast<int>(rhs)); }
inline eParticleBuffer operator&(eParticleBuffer const lhs, int const rhs) { return static_cast<eParticleBuffer> (static_cast<int>(lhs) & rhs); }
inline eParticleBuffer operator^(int const lhs, eParticleBuffer const rhs) { return static_cast<eParticleBuffer> (lhs ^  static_cast<int>(rhs)); }
inline eParticleBuffer operator^(eParticleBuffer const lhs, int const rhs) { return static_cast<eParticleBuffer> (static_cast<int>(lhs) ^ rhs); }
inline eParticleBuffer operator<<(eParticleBuffer const lhs, eParticleBuffer const rhs) { return static_cast<eParticleBuffer> (static_cast<int>(lhs) << static_cast<int>(rhs)); }
inline eParticleBuffer operator>>(eParticleBuffer const lhs, eParticleBuffer const rhs) { return static_cast<eParticleBuffer> (static_cast<int>(lhs) >> static_cast<int>(rhs)); }