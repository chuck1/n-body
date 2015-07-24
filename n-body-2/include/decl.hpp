
#ifndef DECL_HPP
#define DECL_HPP

struct kFrame;
struct kBody;
struct kBranches;
struct kMap;
struct kCollisionBuffer;

struct Frame;
struct Body;
struct Branches;
struct Map;
struct CollisionBuffer;

#ifdef CPU
#define __kernel
#define __local
#define __global
#endif

#endif

