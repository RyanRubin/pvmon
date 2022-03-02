async function Home(resolve) {
    const response = await axios.get('components/home.html');
    resolve({
        template: response.data,
        data: function () {
            return {
                shared: shared,
                password: '',
                isShowPassword: false
            };
        },
        mounted: function () {
            shared.detachListener1();
            shared.detachListener2();
        },
        methods: {
            showPassword: function () {
                this.isShowPassword = !this.isShowPassword;
            },
            login: async function () {
                if (!this.password) {
                    alert('Password required');
                    return;
                }
                try {
                    await auth.signInWithEmailAndPassword('user1@rrtechpvmon.com', this.password);

                    location = '#/main';
                } catch (ex) {
                    alert(ex.message);
                }
            }
        }
    });
}
