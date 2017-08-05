#ifndef OM_MIXIN_HPP_

namespace Om {

/// User can add functionality to a class by specializing a `Mixin` template.
/// In Om, some wrapper template classes publicly inherit from a mixin class template. Users can extend the Om wrapper
/// type by specializing the mixin template type.
///
/// Example:
/// ```
/// template <typename T> class XMixin : public Om::Mixin {}; // base mixin
///
/// template <typename T>
/// class X : public XMixin<T> {};
///
/// class MyType;
///
/// // specialization adds functionality
/// template <>
/// class XMixin<MyType> {
/// 	void do_something() {}
/// };
///
/// X<MyType> x.do_something(); // the extension is callable.
/// ```
///
/// @see `Om::Policy`

class MetaMixin {};

}  // namespace Om

#endif  // OM_MIXIN_HPP_