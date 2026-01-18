/**
 * File   : Lifetime.hpp
 * License: MIT
 * Author : GitHub Copilot
 * Date   : 18/01/2026
 */

#ifndef RTYPE_LIFETIME_HPP_
    #define RTYPE_LIFETIME_HPP_

namespace rtp::ecs::components {
    /**
     * @brief Component that tracks entity lifetime
     * Entity will be despawned when remaining time reaches 0
     */
    struct Lifetime {
        float duration;     /**< Total lifetime duration in seconds */
        float elapsed;      /**< Time elapsed since spawn in seconds */
    };
}

#endif /* !RTYPE_LIFETIME_HPP_ */
