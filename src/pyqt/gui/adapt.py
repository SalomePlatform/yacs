class AdaptationError(TypeError):
        pass
class LiskovViolation(AdaptationError):
        pass

_adapter_factory_registry = {}

def registerAdapterFactory(objtype, protocol, factory):
        _adapter_factory_registry[objtype, protocol] = factory

def unregisterAdapterFactory(objtype, protocol):
        del _adapter_factory_registry[objtype, protocol]

def _adapt_by_registry(obj, protocol, alternate):
        factory = _adapter_factory_registry.get((type(obj), protocol))
        if factory is None:
            adapter = alternate
        else:
            adapter = factory(obj, protocol, alternate)
        if adapter is AdaptationError:
            raise AdaptationError,obj
        else:
            return adapter


def adapt(obj, protocol, alternate=AdaptationError):

        t = type(obj)

        # (a) first check to see if object has the exact protocol
        if t is protocol:
           return obj

        try:
            # (b) next check if t.__conform__ exists & likes protocol
            conform = getattr(t, '__conform__', None)
            if conform is not None:
                result = conform(obj, protocol)
                if result is not None:
                    return result

            # (c) then check if protocol.__adapt__ exists & likes obj
            adapt = getattr(type(protocol), '__adapt__', None)
            if adapt is not None:
                result = adapt(protocol, obj)
                if result is not None:
                    return result
        except LiskovViolation:
            pass
        else:
            # (d) check if object is instance of protocol
            try:
               if isinstance(obj, protocol):
                   return obj
            except:
               pass

        # (e) last chance: try the registry
        return _adapt_by_registry(obj, protocol, alternate)
