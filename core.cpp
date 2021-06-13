#include "core.hpp"
#include "utility.hpp"
#include "printer.hpp"
#include "reader.hpp"

namespace op
{

MalType add(MalType args) {
    auto a = to_array_args<2>(args);
    if (a[0].get_if<MalInt>() && a[1].get_if<MalInt>()) {
        return MalInt(a[0].get<MalInt>() + a[1].get<MalInt>());
    }
    if (a[0].get_if<MalInt>() && a[1].get_if<MalFloat>()) {
        return MalFloat(a[0].get<MalInt>() + a[1].get<MalFloat>());
    }
    if (a[0].get_if<MalFloat>() && a[1].get_if<MalInt>()) {
        return MalFloat(a[0].get<MalFloat>() + a[1].get<MalInt>());
    }
    if (a[0].get_if<MalFloat>() && a[1].get_if<MalFloat>()) {
        return MalFloat(a[0].get<MalFloat>() + a[1].get<MalFloat>());
    }
    if (a[0].get_if<MalString>() && a[1].get_if<MalString>()) {
        return MalString(a[0].get<MalString>() + a[1].get<MalString>());
    }
    throw std::runtime_error("eval error: add: invalid arguments");
}

MalType sub(MalType args) {
    auto a = to_array_args<2>(args);
    if (a[0].get_if<MalInt>() && a[1].get_if<MalInt>()) {
        return MalInt(a[0].get<MalInt>() - a[1].get<MalInt>());
    }
    if (a[0].get_if<MalInt>() && a[1].get_if<MalFloat>()) {
        return MalFloat(a[0].get<MalInt>() - a[1].get<MalFloat>());
    }
    if (a[0].get_if<MalFloat>() && a[1].get_if<MalInt>()) {
        return MalFloat(a[0].get<MalFloat>() - a[1].get<MalInt>());
    }
    if (a[0].get_if<MalFloat>() && a[1].get_if<MalFloat>()) {
        return MalFloat(a[0].get<MalFloat>() - a[1].get<MalFloat>());
    }
    throw std::runtime_error("eval error: sub: invalid arguments");
}

MalType mul(MalType args) {
    auto a = to_array_args<2>(args);
    if (a[0].get_if<MalInt>() && a[1].get_if<MalInt>()) {
        return MalInt(a[0].get<MalInt>() * a[1].get<MalInt>());
    }
    if (a[0].get_if<MalInt>() && a[1].get_if<MalFloat>()) {
        return MalFloat(a[0].get<MalInt>() * a[1].get<MalFloat>());
    }
    if (a[0].get_if<MalFloat>() && a[1].get_if<MalInt>()) {
        return MalFloat(a[0].get<MalFloat>() * a[1].get<MalInt>());
    }
    if (a[0].get_if<MalFloat>() && a[1].get_if<MalFloat>()) {
        return MalFloat(a[0].get<MalFloat>() * a[1].get<MalFloat>());
    }
    throw std::runtime_error("eval error: mul: invalid arguments");
}

MalType div(MalType args) {
    auto a = to_array_args<2>(args);
    if (a[0].get_if<MalInt>() && a[1].get_if<MalInt>()) {
        return MalInt(a[0].get<MalInt>() / a[1].get<MalInt>());
    }
    if (a[0].get_if<MalInt>() && a[1].get_if<MalFloat>()) {
        return MalFloat(a[0].get<MalInt>() / a[1].get<MalFloat>());
    }
    if (a[0].get_if<MalFloat>() && a[1].get_if<MalInt>()) {
        return MalFloat(a[0].get<MalFloat>() / a[1].get<MalInt>());
    }
    if (a[0].get_if<MalFloat>() && a[1].get_if<MalFloat>()) {
        return MalFloat(a[0].get<MalFloat>() / a[1].get<MalFloat>());
    }
    throw std::runtime_error("eval error: div: invalid arguments");
}

MalType list(MalType args) {
    return args;
}

MalType listp(MalType args) {
    auto a = to_array_args<1>(args);
    if (a[0].get_if<MalList>()) {
        return MalBool(true);
    } else {
        return MalBool(false);
    }
}

MalType emptyp(MalType args) {
    auto a = to_array_args<1>(args);
    if (auto listp = a[0].get_if<MalList>()) {
        return listp->empty();
    } else if (auto vectorp = a[0].get_if<MalVector>()) {
        return vectorp->empty();
    } else if (auto mapp = a[0].get_if<MalMap>()) {
        return mapp->empty();
    } else if (a[0].get_if<MalNil>()) {
        return MalType(true);
    }
    return MalBool(false);
}

MalType count(MalType args) {
    auto a = to_array_args<1>(args);
    // FIXME: avoid overflow
    if (auto listp = a[0].get_if<MalList>()) {
        return MalInt(std::distance(listp->begin(),listp->end()));
    } else if (auto vectorp = a[0].get_if<MalVector>()) {
        return MalInt(vectorp->size());
    } else if (auto mapp = a[0].get_if<MalMap>()) {
        return MalInt(mapp->size());
    } else if (a[0].get_if<MalNil>()) {
        return MalInt(0);
    }
    return MalInt(1);
}

MalType eq(MalType args) {
    auto a = to_array_args<1>(args);
    throw NotImplementedException();
    // return MalBool(a[0] == a[1]);
}

MalType ne(MalType args) {
    auto a = to_array_args<1>(args);
    throw NotImplementedException();
    // return MalBool(a[0] != a[1]);
}

MalType lt(MalType args) {
    auto a = to_array_args<1>(args);
    throw NotImplementedException();
    // return MalBool(a[0] < a[1]);
}

MalType le(MalType args) {
    auto a = to_array_args<1>(args);
    throw NotImplementedException();
    // return MalBool(a[0] <= a[1]);
}

MalType gt(MalType args) {
    auto a = to_array_args<1>(args);
    throw NotImplementedException();
    // return MalBool(a[0] > a[1]);
}

MalType ge(MalType args) {
    auto a = to_array_args<1>(args);
    throw NotImplementedException();
    // return MalBool(a[0] >= a[1]);
}

MalType pr_str(MalType args) {
    auto a = to_vector_args(args);
    MalString s;
    if (a.size() == 0) {
        return s;
    }
    s = ::pr_str(a[0]);
    for (std::size_t i = 1; i < a.size(); ++i) {
        s += " ";
        s += ::pr_str(a[i]);
    }
    return s;
}

MalType str(MalType args) {
    auto a = to_vector_args(args);
    MalString s;
    if (a.size() == 0) {
        return s;
    }
    s = ::pr_str(a[0], true);
    for (std::size_t i = 1; i < a.size(); ++i) {
        s += " ";
        s += ::pr_str(a[i], true);
    }
    return s;
}

MalType prn(MalType args) {
    auto a = to_vector_args(args);
    MalString s;
    if (a.size() == 0) {
        return s;
    }
    s = ::pr_str(a[0], true);
    for (std::size_t i = 1; i < a.size(); ++i) {
        s += " ";
        s += ::pr_str(a[i], true);
    }
    std::cout << s << std::endl;
    return MalNil();

}

MalType println(MalType args) {
    auto a = to_vector_args(args);
    MalString s;
    if (a.size() == 0) {
        return s;
    }
    s = ::pr_str(a[0], false);
    for (std::size_t i = 1; i < a.size(); ++i) {
        s += " ";
        s += ::pr_str(a[i], false);
    }
    std::cout << s << std::endl;
    return MalNil();
}

} // namespace op

EnvPtr make_core_env() {
	static EnvPtr penv = std::make_shared<Env>();
	penv->set("+", MalFunction(op::add));
	penv->set("-", MalFunction(op::sub));
	penv->set("*", MalFunction(op::mul));
	penv->set("/", MalFunction(op::div));
	penv->set("add", MalFunction(op::add));
	penv->set("sub", MalFunction(op::sub));
	penv->set("mul", MalFunction(op::mul));
	penv->set("div", MalFunction(op::div));
	penv->set("list", MalFunction(op::list));
	penv->set("listp", MalFunction(op::listp));
	penv->set("count", MalFunction(op::count));
	penv->set("emptyp", MalFunction(op::emptyp));
	penv->set("eq", MalFunction(op::eq));
	penv->set("ne", MalFunction(op::ne));
	penv->set("lt", MalFunction(op::lt));
	penv->set("le", MalFunction(op::le));
	penv->set("gt", MalFunction(op::gt));
	penv->set("ge", MalFunction(op::ge));
	penv->set("prn", MalFunction(op::prn));
	penv->set("pr_str", MalFunction(op::pr_str));
	penv->set("str", MalFunction(op::str));
	penv->set("println", MalFunction(op::println));
    return penv;
}

EnvPtr & get_core_env() {
	static EnvPtr penv = make_core_env();
    return penv;
}

EnvPtr & ns = get_core_env();