firebase.initializeApp({
    apiKey: 'AIzaSyCfpYQawZ_RPrbuAQzVcK5bpj-muvKGYHw',
    authDomain: 'rrtechsolar-pvmon.firebaseapp.com',
    projectId: 'rrtechsolar-pvmon',
    storageBucket: 'rrtechsolar-pvmon.appspot.com',
    messagingSenderId: '409915820731',
    appId: '1:409915820731:web:391dc61189e4482a49779e'
});

const auth = firebase.auth();
const firestore = firebase.firestore();

const shared = {
    detachListener1: function () {},
    detachListener2: function () {}
};

const router = new VueRouter({
    routes: [{
        path: '/',
        component: Home
    }, {
        path: '/main',
        component: Main
    }]
});

new Vue({
    el: '#app',
    router: router,
    data: function () {
        return {
            shared: shared
        };
    }
});
